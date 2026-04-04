#include "NullPhysicsService.h"
#include <Rhea/Core/Controller.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Physics/PhysicsEvent.h>
#include "NullPhysicsCharacter.h"
#include <Psio/Log.h>
#include <RheaPhysics/PhysicsComponent.h>
#include <Rhea/Physics/PhysicsManagerService.h>

namespace rhea::physics
{
	void NullPhysicsService::onPhysicsStep(float delta)
	{
		// send pre event
		std::unique_ptr<rhea::PhysicsEvent> ev = std::make_unique<rhea::PhysicsEvent>("physics_pre", delta);
		sendEvent(std::string("pre_physics"), ev.get());
	}

	void NullPhysicsService::setScene(rhea::Scene* scene)
	{
		// get items of scene
		std::vector<rhea::Item*> items = scene->getItems();

		for (auto it = items.begin(); it != items.end(); ++it) {
			rhea::Item* item = *it;
			if (item->getComponent<PhysicsComponent>())
				continue;

			initializeItem(item);
		}
	}

	void NullPhysicsService::initializeItem(rhea::Item* item)
	{
		auto* mgr = doCast<rhea::PhysicsManagerService>(m_app->getService("physics_mgr"));
		const auto* desc = mgr->getBodyDescFor(item);
		if (!desc || desc->kind.empty()) return;

		// we got a description -> add a body
		auto* pc = item->getComponent<PhysicsComponent>();
		if (!pc) {
			pc = item->addComponentUnique<PhysicsComponent>();
		}		
		pc->setBody(std::make_unique<NullPhysicsCharacter>(item));
	}

	void NullPhysicsService::removeItem(rhea::Item* item)
	{
		item->deleteComponent<PhysicsComponent>();
	}
	
	void NullPhysicsService::onInit()
	{
		m_app->getUnitTree()->registerEventHandler("scene_activate", this, [&](rhea::Event* ev, rhea::Unit* tree)->bool {
			setScene((rhea::Scene*)ev->getTarget());
			return false;
			});
	}
}