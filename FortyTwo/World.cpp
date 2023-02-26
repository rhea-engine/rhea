#include "World.h"
#include <Psio/Log.h>
#include <Rhea/Physics/PhysicsService.h>
#include <Rhea/Physics/PhysicsManager.h>
#include "ServerService.h"
#include <Rhea/Platform/PlatformService.h>
#include <Rhea/ResourceList.h>
#include <Rhea/UnitTree.h>

void World::setScene(std::string scene)
{
	m_scene_name = scene;
}

void World::setup(Rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
{
	Resource::setup(app, attributes, values);

	if (m_values.find("avatarlist") != m_values.end()) {
		if (!m_resource_list)
			m_resource_list = new Rhea::ResourceList(app);
		m_resource_list->push("avatarlist", m_values["avatarlist"], "//meworla/avatarlist");
	}

	if (m_values.find("scene") == m_values.end()) {
		LOG(LOG_ERROR) << "No scene defined. Exiting.";
		//exit(1);
	}
	else {
		if (!m_resource_list)
			m_resource_list = new Rhea::ResourceList(app);
		m_resource_list->push("scene", m_values["scene"], "//meworla/scene");
	}
}

Rhea::Unit* World::getUserInfo(std::string username)
{
	Rhea::Unit* a_list = (*m_resource_list)["avatarlist"];
	if (a_list) {
		Rhea::Unit* user_info = a_list->getChild(username);
		return user_info;
	}
	else
		return nullptr;
}

void World::ready()
{
	m_scene = (Rhea::Scene*)(*m_resource_list)["scene"];

	if (m_scene) {

		Rhea::PhysicsService* physics = static_cast<Rhea::PhysicsService*>(m_app->getService("physics"));
		if (!physics) {
			Variant phy = m_scene->getValue("physics");
			if (phy == std::string("none")) {
				physics = static_cast<Rhea::PhysicsManagerService*>(m_app->getService("physics_mgr"))->createPhysicsService(Rhea::PhysicsManagerService::Types::NONE);
			}
			else {
				physics = static_cast<Rhea::PhysicsManagerService*>(m_app->getService("physics_mgr"))->createPhysicsService(Rhea::PhysicsManagerService::Types::WORLD);
			}
			physics->onInit();
			m_app->registerService("physics", physics);
		}

		static_cast<Rhea::PhysicsService*>(m_app->getService("physics"))->setScene(static_cast<Rhea::Scene*>(m_scene));

		// now we are ready
		std::unique_ptr<Rhea::Event> ev_r = std::make_unique<Rhea::Event>("ready", nullptr);
		m_scene->sendEvent(std::string("ready"), ev_r.get());
	}
	else {
		LOG(LOG_ERROR) << "World::ready(): Could not load scene";	
	}
}

Rhea::Unit* World::createType(std::string name, std::string type)
{
	Unit* ret = (Unit*)new World(name, type);
	return ret;
}