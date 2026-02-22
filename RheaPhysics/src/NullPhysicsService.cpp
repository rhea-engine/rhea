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

namespace Rhea
{
	void NullPhysicsService::onPhysicsStep(float delta)
	{
		// send pre event
		std::unique_ptr<Rhea::PhysicsEvent> ev = std::make_unique<Rhea::PhysicsEvent>("physics_pre", delta);
		sendEvent(std::string("pre_physics"), ev.get());
	}

	void NullPhysicsService::setScene(Scene* scene)
	{
		// get items of scene
		std::vector<Item*> items = scene->getItems();

		for (auto it = items.begin(); it != items.end(); ++it) {
			Item* item = *it;
			if (item->getComponent<PhysicsComponent>())
				continue;

			initializeItem(item);
		}
	}

	void NullPhysicsService::initializeItem(Item* item)
	{
		auto* mgr = doCast<PhysicsManagerService>(m_app->getService("physics_mgr"));
		const auto* desc = mgr->getBodyDescFor(item);
		if (!desc || desc->kind.empty()) return;

		// we got a description -> add a body
		auto* pc = item->getComponent<PhysicsComponent>();
		if (!pc) {
			pc = item->addComponentUnique<PhysicsComponent>();
		}		
		pc->setBody(std::make_unique<NullPhysicsCharacter>(item));
	}

	void NullPhysicsService::removeItem(Item* item)
	{
		item->deleteComponent<PhysicsComponent>();
	}
	
	void NullPhysicsService::onInit()
	{
		m_app->getUnitTree()->registerEventHandler("scene_activate", this, [&](Event* ev, Unit* tree)->bool {
			setScene((Scene*)ev->getTarget());
			return false;
			});
	}
}