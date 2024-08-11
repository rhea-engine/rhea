#ifndef RHEA_GLTF_LOADER_H
#define RHEA_GLTF_LOADER_H

#include "Resources/Resource.h"
#include "Resources/ResourceLoader.h"

namespace Rhea
{
	class GltfLoader
	{
	public:
		static void load(std::string name, Application* app, Orion::ResourceRequest* req, Resource::LoadedHandler handler) {
			load_impl(name, app, req, handler);
		}
		static void load_impl(std::string name, Application* app, Orion::ResourceRequest* req, Resource::LoadedHandler handler, bool is_binary = false);
	};
}
#endif // !RHEA_GLTF_LOADER_H
