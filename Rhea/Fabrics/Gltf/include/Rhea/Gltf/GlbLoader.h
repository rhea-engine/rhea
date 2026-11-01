/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GLB_LOADER_H
#define RHEA_GLB_LOADER_H

#include "GltfLoader.h"
#include <Rhea/Resources/Resource.h>

namespace rhea
{
	class GlbLoader
	{
	public:
		static void load(const std::string& name, const std::string& res_id, Application* app, orion::ResourceRequest* req, ResourceContext context, Resource::LoadedHandler handler) {
			GltfLoader::load_impl(name, res_id, app, req, context, handler, true);
		}
	};
}
#endif // !RHEA_GLB_LOADER_H
