/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_SCENELOADER_H
#define RHEA_SCENELOADER_H

#include <Rhea/Resources/Resource.h>
#include <Rhea/Resources/ResourceLoader.h>

namespace psio
{
	class IOBuffer;
}

namespace rhea
{
	class Application;
	class Unit;
	class Scene;

	class SceneLoader
	{
	public:
		static void load(std::string name, Application* app, orion::ResourceRequest* req, Resource::LoadedHandler handler);
		static void parseIniFile(std::string name, Application* app, orion::ResourceRequest* req, Resource::LoadedHandler handler, psio::IOBuffer* buf, size_t buflen);
	};
}
#endif // !RHEA_SCENELOADER_H
