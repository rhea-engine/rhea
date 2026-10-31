/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GLTF_LOADER_H
#define RHEA_GLTF_LOADER_H

#include <Rhea/Resources/Resource.h>
#include <Rhea/Resources/ResourceLoader.h>

namespace rhea
{
	class GltfLoader
	{
	public:
		static void load(std::string name, Application* app, orion::ResourceRequest* req, ResourceContext context, Resource::LoadedHandler handler) {
			load_impl(name, app, req, context, handler);
		}
		static void load_impl(std::string name, Application* app, orion::ResourceRequest* req, ResourceContext context, Resource::LoadedHandler handler, bool is_binary = false);
	};
}
#endif // !RHEA_GLTF_LOADER_H
