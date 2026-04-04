#ifndef RHEA_GLTF_LOADER_H
#define RHEA_GLTF_LOADER_H

#include <Rhea/Resources/Resource.h>
#include <Rhea/Resources/ResourceLoader.h>

namespace rhea
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
