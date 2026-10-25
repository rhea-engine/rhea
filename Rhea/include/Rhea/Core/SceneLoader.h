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
#include <Rhea/Resources/ResourceContext.h>
#include <Rhea/Core/SceneResource.h>

namespace psio
{
	class IOBuffer;
}

namespace rhea
{
	class Application;
	class Unit;
	class SceneResource;

	class SceneLoader
	{
	public:
		static void load(std::string name, Application* app, orion::ResourceRequest* req, ResourceContext& context, Resource::LoadedHandler handler);
	private:
		// build a SceneResource from a resource request
		static Error parseIniFile(std::string name, char* buf, size_t buflen, SceneResource::SceneDefinition& definition);
	};
}
#endif // !RHEA_SCENELOADER_H
