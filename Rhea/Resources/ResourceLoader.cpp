/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Resources/ResourceLoader.h>

namespace rhea
{
	std::unordered_map<std::string, ResourceLoader::LoadFunc> ResourceLoader::m_loaders;

	bool ResourceLoader::load(const std::string& name, const std::string& rid, Application* app, const std::string& mime_type, orion::ResourceRequest* req, const ResourceContext& context, Resource::LoadedHandler handler)
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
		l->second(name, rid, app, req, context, handler);
		return true;
	}
}