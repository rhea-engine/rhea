#ifndef RHEA_GLB_LOADER_H
#define RHEA_GLB_LOADER_H

#include "GltfLoader.h"
#include <Rhea/Resources/Resource.h>

namespace Rhea
{
	class GlbLoader
	{
	public:
		static void load(std::string name, Application* app, Orion::ResourceRequest* req, Resource::LoadedHandler handler) {
			GltfLoader::load_impl(name, app, req, handler, true);
		}
	};
}
#endif // !RHEA_GLB_LOADER_H
