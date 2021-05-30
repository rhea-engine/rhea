#include "ResourceLoader.h"

namespace Rhea
{
	std::unordered_map<std::string, ResourceLoader::LoadFunc> ResourceLoader::m_loaders;

	bool ResourceLoader::load(std::string name, Application* app, std::string mime_type, Orion::ResourceRequest* req, Resource::LoadedHandler handler)
	{
		auto l = m_loaders.find(mime_type);
		if (l == m_loaders.end()) {
			// type not found - try base
			std::string generic(mime_type.substr(0, mime_type.find_first_of("/")) + "/*");
			l = m_loaders.find(generic);
			if (l == m_loaders.end()) {
				// also no generic handler found
				return false;
			}
		}

		// call load on loader
		l->second(name, app, req, handler);
		return true;
	}
}