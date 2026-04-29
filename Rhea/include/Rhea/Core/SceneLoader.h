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

namespace rhea
{
	class Application;
	class Unit;
	class Scene;

	class SceneLoader
	{
	public:
		static void load(std::string name, Application* app, orion::ResourceRequest* req, Resource::LoadedHandler handler);
		//void load(Scene* scene, orion::Resource* resource, Application* app);
	};
}
#endif // !RHEA_SCENELOADER_H
