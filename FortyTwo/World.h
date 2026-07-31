/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef FORTYTWO_WORLD_H
#define FORTYTWO_WORLD_H

#include <string>
#include <Rhea/Application.h>
#include <Rhea/Resources/Resource.h>

class World : public rhea::Unit
{
	MECLASS(World, rhea::Unit)
public:
	World(std::string name, std::string type) :
		m_scene_ready(false),
		m_scene(nullptr),
		m_a_list(nullptr),
		rhea::Unit(name,type)
	{}

	void setScene(std::string scene);

	// start the setup process
	void setup(rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

	std::vector<Unit::ResourceReference> getReferences();

	rhea::Scene* getScene() { return m_scene; }

	// ready event
	void ready();

protected:
	std::string m_scene_name;
	rhea::Scene* m_scene;
	bool m_scene_ready;
	rhea::Unit* m_a_list;
};

#endif