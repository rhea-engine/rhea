/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#ifndef RHEA_WORLD_H
#define RHEA_WORLD_H

#include <Rhea/Core/Scene.h>
#include <Rhea/Ref.h>

namespace rhea
{
	class World : public Scene
	{
		RECLASS(World, Scene, "//meworla/world");
	public:
		World(std::string name, std::string type_name) :
			Scene(name, type_name)
		{}
		void setup(rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, simpleparser::Variant> values) override;

		Ref<Scene> getStartScene() const;
		Unit* getPersistentState();
	};
}

#endif // !RHEA_WORLD_H
