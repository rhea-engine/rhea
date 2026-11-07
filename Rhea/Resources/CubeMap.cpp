/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Resources/CubeMap.h"
#include <Psio/Log.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>

namespace rhea
{
	void CubeMap::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		ImageResource::setup(app, attributes, values);

		if (m_values.find("path") == m_values.end()) {

			for (size_t i = 0; i < 6; ++i)
				if (m_values.find("cubemap" + std::to_string(i + 1)) != m_values.end())
					m_cubemap_refs[i].setId(values["cubemap" + std::to_string(i + 1)].toString());
				else {
					LOG(LOG_ERROR) << "CubeMap missing cubemap" << (i + 1) << " attribute.";
				}
		}
	}

	std::vector<Component::ResourceReference> CubeMap::getReferences()
	{
		auto references = ImageResource::getReferences();

		for (size_t i = 0; i < 6; ++i) {
			if (!m_cubemap_refs[i].empty()) {
				references.push_back({
					"cubemap_" + std::to_string(i),
					"//meworla/texture",
					m_cubemap_refs[i].id()
					});
			}
		}
		return references;
	}

	void CubeMap::resolveReferences(const std::string& context_root, const std::string& source_root)
	{
		ImageResource::resolveReferences(context_root, source_root);

		const ResourceContext context{ context_root, source_root };
		for (auto& reference : m_cubemap_refs) {
			if (!reference.empty())
				normalizeReference(reference, context);
		}
	}

	void CubeMap::instanceSetup(Component* source, Component* target, InstantiationContext& context)
	{
		ImageResource::instanceSetup(source, target, context);

		const auto* source_cubemap = doCast<CubeMap>(source);
		auto* target_cubemap = doCast<CubeMap>(target);

		target_cubemap->m_cubemap_refs = source_cubemap->m_cubemap_refs;
	}

	void CubeMap::ready()
	{
		if (m_status >= Status::READY)
			return;
		m_status = Status::READY;
		Unit::ready();

		if (m_values.find("path") != m_values.end()) {
			ImageResource* img = m_app->getUnitTree()->resolve(m_image_ref);
			m_images.push_back(img);
		}
		else
		{
			m_images.resize(6);
			for (int i = 1; i <= 6; i++) {
				if (m_values.find("cubemap" + std::to_string(i)) != m_values.end()) {
					ImageResource* img = m_app->getUnitTree()->resolve(m_cubemap_refs[i - 1]);
					if (img) {
						m_images[i - 1] = img;
					}
					else {
						LOG(LOG_ERROR) << "Failed to load image " << (std::string)(m_values["cubemap" + std::to_string(i)]);
						return;
					}
				}
			}
		}
		m_setup = true;
	}

	Resource* CubeMap::getImage(int index)
	{
		if (!m_setup)
			return nullptr;
		if (index < 0 || index >= m_images.size())
			return nullptr;
		return m_images[index];
	}
}