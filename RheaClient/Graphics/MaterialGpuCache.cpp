#include <Rhea/Graphics/MaterialGpuCache.h>
#include <Rhea/Resources/ImageResource.h>

namespace Rhea
{
	Poseidon::Material* MaterialGpuCache::getOrCreate(Poseidon::Core* core, Material* material, const Builder& builder)
	{
        auto key = material->getPath();
        std::string desiredShader = material->getValue("shader");
        auto desiredTexKeys = collectTexKeys(material);

        auto it = m_entries.find(key);
        bool fresh = false;

        if (it == m_entries.end() || it->second.textureKeys != desiredTexKeys || it->second.shaderName != desiredShader) {
            Entry e;
            e.mat.reset(core->createMaterial());
            std::string mat_name = material->getPathName();
            core->registerMaterial(mat_name, e.mat.get());   // is this needed?
            e.shaderName = desiredShader;
            e.textureKeys = desiredTexKeys;
            it = m_entries.insert_or_assign(key, std::move(e)).first;
            fresh = true;
        }

        if (!builder(core, material, it->second.mat.get(), fresh)) return nullptr;
        return it->second.mat.get(); 
	}

    std::vector<std::string> MaterialGpuCache::collectTexKeys(Material* material)
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
                    auto* child = doCast<Rhea::ImageResource>(img->getImage(i));
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
