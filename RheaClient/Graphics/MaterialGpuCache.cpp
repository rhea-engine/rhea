/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <RheaClient/Graphics/MaterialGpuCache.h>
#include <Rhea/Resources/ImageResource.h>
#include <Rhea/Core/Shader.h>
#include <Psio/Log.h>

namespace rhea::client {
	poseidon::Material* MaterialGpuCache::getOrCreate(poseidon::Core* core, rhea::Material* material, const Builder& builder)
	{
        auto key = material->getPath();
#ifdef _DEBUG
        if (key.substr(0,2) != "//")
			DLOG(LOG_DEBUG) << "MaterialGpuCache::getOrCreate: material path '" << key << "' does not start with '//', this may cause cache misses";
#endif
        std::string desiredShader;
		Shader* shader = material->getShader();
        if (shader)
			desiredShader = shader->getPathName();
        else
            desiredShader = material->getValueString("shader");
        auto desiredTexKeys = collectTexKeys(material);

        auto [it, inserted] = m_entries.try_emplace(key);
        auto& entry = it->second;

        if (inserted)
            entry.mat.reset(core->createMaterial());

        const bool changed = inserted || entry.textureKeys != desiredTexKeys || entry.shaderName != desiredShader;

        if (!builder(core, material, entry.mat.get(), changed))
            return nullptr;

        entry.textureKeys = std::move(desiredTexKeys);
        entry.shaderName = desiredShader;

        return entry.mat.get();
	}

    std::vector<std::string> MaterialGpuCache::collectTexKeys(rhea::Material* material)
    {
        std::vector<std::string> keys;

        auto textures = material->getTextures();
        keys.reserve(textures.size());

        for (auto* img : textures) {
            if (!img) {
                keys.emplace_back("<null>");
                continue;
            }

            std::string k = img->getPath();

            // Fallback if pathName is empty            
            if (k.empty()) {
                // Last-resort: keep ordering stable but avoid empty
                k = "<unnamed-image>";
            }

            // If this is a multi-image (cubemap/array), include child keys too.
            // This prevents missing rebuilds when one face changes.
            if (img->getNumImages() > 1) {
                k += "|multi(" + std::to_string(img->getNumImages()) + "):";
                for (int i = 0; i < img->getNumImages(); ++i) {
                    auto* child = doCast<rhea::ImageResource>(img->getImage(i));
                    if (child) {
                        k += child->getPath();
                    }
                    else {
                        k += "<bad-child>";
                    }
                    k += ";";
                }
            }

            keys.push_back(std::move(k));
        }
        return keys;
    }
}
