/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Physics/PhysicsManagerService.h>
#include <Rhea/Physics/PhysicsService.h>
#include <Rhea/Physics/PhysicsBody.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Core/WorldService.h>
#include <Psio/Log.h>

namespace rhea
{
	PhysicsManagerService::PhysicsManagerService(std::string name, std::string type_name)
		: Service(std::move(name), std::move(type_name))
	{
	}

	void PhysicsManagerService::registerBackend(const std::string& id, BackendCreator creator)
	{
		m_backend_creators[id] = std::move(creator);
	}

	void PhysicsManagerService::registerBodyForType(simpleparser::Type* typedesc, PhysicsBodyDesc desc)
	{
		if (!typedesc) return;
		m_type_to_body[typedesc] = std::move(desc);
	}

	const PhysicsBodyDesc* PhysicsManagerService::getBodyDescFor(Item* item) const
	{
		if (!item) return nullptr;
		simpleparser::Type* t = item->getType();
		while (t) {
			auto it = m_type_to_body.find(t);
			if (it != m_type_to_body.end())
				return &it->second;
			t = t->getParent();
		}
		return nullptr;
	}

	void PhysicsManagerService::registerPhysicsComponent(const std::string& name, ComponentCreator creator)
	{
		m_component_creators[name] = std::move(creator);

		// if a service already exists, forward register immediately
		if (m_phy_service) {
			m_phy_service->registerPhysicsComponent(name, m_component_creators[name]);
		}
	}

	PhysicsService* PhysicsManagerService::createPhysicsService(const std::string& backend_id)
	{
		auto it = m_backend_creators.find(backend_id);
		if (it == m_backend_creators.end()) return nullptr;		

		m_phy_service = it->second(m_app);         // m_app from Service base
		m_phy_service->setup(m_app);

		// forward all already registered component creators
		for (auto& kv : m_component_creators) {
			m_phy_service->registerPhysicsComponent(kv.first, kv.second);
		}
		
		for (auto& h : m_backend_hooks) h(m_phy_service.get());

		return m_phy_service.get();
	}

	void PhysicsManagerService::onInit()
	{
		// register in worldservice to be called during world prepare
		WorldService* world_service = doCast<WorldService>(m_app->getService("world"));
		if (world_service) {
			world_service->addWorldPrepareHandler([this](rhea::WorldService::WorldPrepareContext* context) -> rhea::Error {
				
				// initialize physics

				std::string backend = "none";
				if (!context || (!context->world) && (!context->standaloneScene)) {
					LOG(LOG_ERROR) << "WorldService has no world and no scene. Physics will be disabled.";
					return rhea::Error(rhea::Error::Code::UNAVAILABLE, "WorldService has no world and no scene. Physics will be disabled.");
				}
				std::string phy = "none";
				if (context->world)
					phy = context->world->getValueString("physics", "none");
				else if (context->standaloneScene)
					phy = context->standaloneScene->getValueString("physics", "none");
				if (phy == std::string("world") || phy == std::string("bullet")) {
					backend = "world";
				}
				else if (phy == std::string("null")) {
					backend = "null";
				}
				else if (!phy.empty()) {
					// allow future backends by id, e.g. "physx"
					backend = phy;
				}

				// if no physics backend is specified or explicitly disabled with "none", we return early and do not initialize physics
				if (backend == "none")
					return rhea::Error();

				// create service by id (manager handles fallback to "none" if unknown/unavailable)
				rhea::PhysicsService* physics = createPhysicsService(backend);
				if (!physics) {
					LOG(LOG_ERROR) << "Failed to create physics service for backend: " << backend;
					return rhea::Error(rhea::Error::Code::UNAVAILABLE, "Failed to create physics service for backend: " + backend);
				}
				if (physics) {
					physics->onInit();
					m_app->setPhysicsService(physics);
					return rhea::Error();
				}

				return rhea::Error(rhea::Error::Code::UNAVAILABLE, "Physics initialization failed");
				});
		}
	}

	// note: currently physics_service is stored directly in application
	bool PhysicsManagerService::onPrePhysics(float delta)
	{
		if (m_phy_service)
			return m_phy_service->onPrePhysics(delta);
		return false;
	}

	void PhysicsManagerService::onPhysicsStep(float frame_slice)
	{
		if (m_phy_service)
			m_phy_service->onPhysicsStep(frame_slice);
	}

	bool PhysicsManagerService::onFrame(float delta)
	{
		if (m_phy_service)
			return m_phy_service->onFrame(delta);
		return false;
	}

	bool PhysicsManagerService::onFrameFinal(float delta)
	{
		if (m_phy_service)
			return m_phy_service->onFrameFinal(delta);
		return false;
	}
}
