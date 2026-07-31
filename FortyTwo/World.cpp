/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "World.h"
#include <Psio/Log.h>
#include <Rhea/Physics/PhysicsService.h>
#include <Rhea/Network/ServerService.h>
#include <Rhea/Platform/PlatformService.h>
#include <Rhea/Resources/ResourceList.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Physics/PhysicsManagerService.h>

void World::setScene(std::string scene)
{
	m_scene_name = scene;
}

void World::setup(rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
{
	Unit::setup(app, attributes, values);	
}

std::vector<Unit::ResourceReference> World::getReferences()
{
	std::vector<ResourceReference> refs = Unit::getReferences();

	if (m_values.find("scene") != m_values.end()) {
		refs.push_back({ "scene", "//meworla/scene", m_values["scene"] });
	}
	else {
		LOG(LOG_ERROR) << "No scene defined. Exiting.";
	}
	return refs;
}

void World::ready()
{
	m_scene = (rhea::Scene*)m_app->getUnitTree()->find(m_values["scenename"].toString(), m_attributes["basepath"], false);

	if (m_scene) {

		auto* mgr = doCast<rhea::PhysicsManagerService>(m_app->getService("physics_mgr"));
		if (mgr && !mgr->getPhysicsService()) {
			std::string backend = "none";
			std::string phy = m_scene->getValueString("physics", "none");
			if (phy == std::string("world") || phy == std::string("bullet")) {
				backend = "world";
			}
			else if (phy == std::string("none")) {
				backend = "none";
			}
			else if (!phy.empty()) {
				// allow future backends by id, e.g. "physx"
				backend = phy;
			}
			// create service by id (manager handles fallback to "none" if unknown/unavailable)
			rhea::PhysicsService* physics = mgr->createPhysicsService(backend);
			if (!physics) {
				// last-resort fallback if the manager doesn't already do it
				physics = mgr->createPhysicsService("none");
			}

			if (physics) {
				physics->onInit();
			}
		}

		m_scene->show();
	}
	else {
		LOG(LOG_ERROR) << "World::ready(): Could not load scene " << (std::string)m_values["scenename"];
	}
}
