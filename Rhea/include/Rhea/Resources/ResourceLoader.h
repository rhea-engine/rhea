/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_RESOURCELOADER_H
#define RHEA_RESOURCELOADER_H

#include <Rhea/Resources/Resource.h>

namespace rhea
{
	class ResourceLoader
	{
	public:
		typedef std::function<void(std::string, Application* app, orion::ResourceRequest*, Resource::LoadedHandler)> LoadFunc;

		// call initialize and register loader by class
		template <class T>
		static void registerLoader(std::string mime_type)
		{
			LoadFunc loader = T::load;
			m_loaders[mime_type] = loader;
			
		}

		// load a resource
		// returns false if no loader found
		bool load(std::string name, Application* app, std::string mime_type, orion::ResourceRequest* req, Resource::LoadedHandler handler);

	protected:

		// loader database
		static std::unordered_map<std::string, LoadFunc> m_loaders;
	};
}

#endif // !RHEA_RESOURCELOADER_H
