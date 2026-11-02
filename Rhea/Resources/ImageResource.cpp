/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Resources/ImageResource.h"
#include "Rhea/UnitTree.h"
#include "Rhea/Resources/ResourceList.h"

#include "Rhea/Application.h"
#include "Orion/ResourceStream.h"
#include <Psio/Log.h>

#include <cstring>

namespace rhea
{
	ImageResource::~ImageResource()
	{
		if (m_buf)
			free(m_buf);
		m_buf = nullptr;
	}

	void ImageResource::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Resource::setup(app, attributes, values);		
	}

	void ImageResource::initializeType(simpleparser::Type* type)
	{
		registerResourceProperty(type, "path", &ImageResource::m_image_ref, "//meworla/image", ResourceUsage::Load);
	}

	void ImageResource::ready()
	{
		if (m_status >= Status::READY)
			return;
		m_status = Status::READY;
		Resource::ready();

		auto path_it = m_values.find("path");
		if (!m_buf && path_it != m_values.end()) {
			ImageResource* img = m_app->getUnitTree()->resolve(m_image_ref);
			if (img && img->m_buflen) {
				m_buf = (char*)malloc(img->m_buflen);
				if (m_buf && img->m_buf != nullptr) {
					memcpy(m_buf, img->m_buf, img->m_buflen);
					m_buflen = img->m_buflen;
				}
				else {
					LOG(LOG_ERROR) << "Failed to load image " << getPathName() << " could not allocate buffers.";
				}
			}
			else {
				LOG(LOG_ERROR) << "Failed to load image " << getPathName() << " image not found or not valid";
				//abort();
			}
		}
	}
}
