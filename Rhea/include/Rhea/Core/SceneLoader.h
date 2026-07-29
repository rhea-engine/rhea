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
		struct NodeDefinition {
			std::string node_id;
			std::string parent_id;
			std::string name;
			std::string type_name;
			std::unordered_map<std::string, std::string> attributes;
			std::unordered_map<std::string, Variant> values;
		};

		struct SceneDefinition {
			NodeDefinition root;
			std::vector<NodeDefinition> nodes;
		};

		static void load(std::string name, Application* app, orion::ResourceRequest* req, Resource::LoadedHandler handler);
		// build a SceneDefinition from a resource request
		static SceneDefinition* parseIniFile(std::string name, const std::string& scene_root, char* buf, size_t buflen);

		static void instantiate(Application* app, Resource::LoadedHandler handler, const std::string& scene_root, SceneDefinition* scene_def);
	};
}
#endif // !RHEA_SCENELOADER_H
