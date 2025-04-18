#include "PhysicsService.h"
#include "Body.h"
#include "../Controller/BasicController.h"
#include "Fabrics/Core/Item.h"
#include "../Application.h"
#include "UnitTree.h"
#include "PhysicsEvent.h"
#include "PhysicsCharacter.h"
#include <Psio/Log.h>
#include <PhysicsComponent.h>

namespace Rhea
{
	void PhysicsService::onPhysicsStep(float delta)
	{
		// send pre event
		std::unique_ptr<Rhea::PhysicsEvent> ev = std::make_unique<Rhea::PhysicsEvent>("physics_pre", delta, m_terrain);
		sendEvent(std::string("pre_physics"), ev.get());

		if (!m_item)
			return;

		bool  dirty = false;
		Math::Vector3 move(m_item->getMovement());
		if (move != Math::Vector3::ZERO) {
			// currently moving			
			move *= delta;
			m_item->getTransform()->get_position()->moveRelative(move);
			dirty = true;
		}
		float m_turn = m_item->getTurn();
		if (m_turn != 0) {
			float turn_step = m_turn * delta;
			Math::Quaternion q;
			q.fromAngleAxis(turn_step, Math::Vector3(0, 1, 0));
			Math::Quaternion m_orientation = q * m_item->getTransform()->get_position()->get_rotation(); //* m_orientation;
			m_orientation.normalize();			
			m_item->getTransform()->get_position()->setOrientation(m_orientation);
			dirty = true;
		}
	}

	void PhysicsService::setScene(Scene* scene)
	{
		Controller* controller = scene->findByType<Controller>();
		if (controller) {
			// get target from controller
			m_item = controller->getTarget();
			if (m_item)
				initializeItem(m_item);
		}
		else {
			LOG(LOG_ERROR) << "No controller found in setting scene";
		}

	}

	void PhysicsService::initializeItem(Item* item)
	{
		PhysicsBody* body = new PhysicsCharacter(item);
		item->addComponentUnique<PhysicsComponent>(body);
	}

	void PhysicsService::removeItem(Item* item)
	{
		item->deleteComponent<PhysicsComponent>();
	}
	
	void PhysicsService::onInit()
	{
		m_app->getUnitTree()->registerEventHandler("scene_activate", this, [&](Event* ev, Unit* tree)->bool {
			setScene((Scene*)ev->getTarget());
			return false;
			});
	}
}