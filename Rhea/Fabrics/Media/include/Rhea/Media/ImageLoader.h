/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_FABRICS_MEDIA_IMAGELOADER_H
#define RHEA_FABRICS_MEDIA_IMAGELOADER_H

#include "Rhea/Resources/Resource.h"
#include "Rhea/Resources/ResourceLoader.h"

namespace rhea
{
	class ImageLoader {
	public:
		static void load(const std::string& name, const std::string& res_id, Application* app, orion::ResourceRequest* req, ResourceContext context, Resource::LoadedHandler handler);
	};
}

#endif // !RHEA_FABRICS_MEDIA_IMAGELOADER_H
