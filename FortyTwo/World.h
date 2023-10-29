#ifndef FORTYTWO_WORLD_H
#define FORTYTWO_WORLD_H

#include <string>
#include <Rhea/Application.h>
#include <Rhea/Resources/Resource.h>

class World : public Rhea::Unit
{
	MECLASS(World, Rhea::Unit)
public:
	World(std::string name, std::string type) :
		m_scene_ready(false),
		m_scene(nullptr),
		m_a_list(nullptr),
		Rhea::Unit(name,type)
	{}

	void setScene(std::string scene);

	// start the setup process
	void setup(Rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

	std::vector<Unit::Export> getReferences();

	Rhea::Scene* getScene() { return m_scene; }

	// get user info by username
	virtual Rhea::Unit* getUserInfo(std::string username);

	// ready event
	void ready();

protected:
	std::string m_scene_name;
	Rhea::Scene* m_scene;
	bool m_scene_ready;
	Rhea::Unit* m_a_list;
};

#endif