#include "World.h"
#include <Psio/Log.h>
#include <Rhea/Physics/PhysicsService.h>
#include <Rhea/Physics/PhysicsManager.h>
#include "ServerService.h"
#include <Rhea/Platform/PlatformService.h>
#include <Rhea/Resources/ResourceList.h>
#include <Rhea/UnitTree.h>

void World::setScene(std::string scene)
{
	m_scene_name = scene;
}

void World::setup(Rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
{
	Unit::setup(app, attributes, values);	
}

std::vector<Unit::Export> World::getReferences()
{
	std::vector<Export> refs = Unit::getReferences();
	if (m_values.find("avatarlist") != m_values.end()) {
		refs.push_back({ "avatarlist", "//meworla/avatarlist", m_values["avatarlist"] });
	}
	if (m_values.find("scene") != m_values.end()) {
		refs.push_back({ "scene", "//meworla/scene", m_values["scene"] });
	}
	else {
		LOG(LOG_ERROR) << "No scene defined. Exiting.";
	}
	return refs;
}


Rhea::Unit* World::getUserInfo(std::string username)
{
	if (!m_a_list && m_values.find("avatarlist")!=m_values.end()) {
		m_a_list = m_app->getUnitTree()->find(m_values["avatarlist"]);
	}
	if (m_a_list) {
		Rhea::Unit* user_info = m_a_list->getChild(username);
		return user_info;
	}
	else
		return nullptr;
}

void World::ready()
{
	m_scene = (Rhea::Scene*)m_app->getUnitTree()->find(m_values["scenename"], m_attributes["basepath"], false);

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

		m_scene->show();
	}
	else {
		LOG(LOG_ERROR) << "World::ready(): Could not load scene " << (std::string)m_values["scenename"];
	}
}
