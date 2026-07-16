/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#include <Rhea/Core/World.h>

namespace rhea
{
	void World::setup(rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, simpleparser::Variant> values)
	{
		Scene::setup(app, attributes, values);

		if (values.find("startscene") != values.end())
		{
			// Handle startscene value
		}
	}

	Ref<Scene> World::getStartScene() const
	{
		return Ref<Scene>();
	}

	Unit* World::getPersistentState()
	{
		return nullptr;
	}
}