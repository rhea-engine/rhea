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
	}

	std::vector<Component::ResourceReference> CubeMap::getReferences()
	{
		std::vector<ResourceReference> refs;
		if (m_values.find("path") != m_values.end()) {
			refs.push_back({ "image", "//meworla/image", m_values["path"] });
		}
		else
		{
			int i;
			for (i = 1; i <= 6; i++) {
				if (m_values.find("cubemap" + std::to_string(i)) != m_values.end()) {
					refs.push_back({ "image" + std::to_string(i), "//meworla/image", m_values["cubemap" + std::to_string(i)] });
				}
				else
				{
					break;
				}
			}
			if (i != 7) {
				LOG(LOG_ERROR) << "CubeMap has less than 6 images";
			}
		}
		return refs;
	}

	void CubeMap::ready()
	{
		if (m_status >= Status::READY)
			return;
		m_status = Status::READY;
		Unit::ready();

		if (m_values.find("path") != m_values.end()) {
			m_images.push_back(doCast<ImageResource>(m_app->getUnitTree()->find(m_values["path"].toString(), m_attributes["basepath"], false)));
		}
		else
		{
			m_images.resize(6);
			for (int i = 1; i <= 6; i++) {
				if (m_values.find("cubemap" + std::to_string(i)) != m_values.end()) {
					ImageResource* img = doCast<ImageResource>(m_app->getUnitTree()->find(m_values["cubemap" + std::to_string(i)].toString(), m_attributes["basepath"], false));
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