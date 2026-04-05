#include <functional>

#include <Rhea/Character/Character.h>
#include <Rhea/Application.h>
#include <Rhea/Service.h>
#include <Rhea/Physics/PhysicsBody.h>
#include <Rhea/Physics/PhysicsEvent.h>
#include <Rhea/Physics/PhysicsManagerService.h>
#include <Rhea/Physics/PhysicsService.h>

using math::Quaternion;
using math::Vector3;

namespace rhea
{
	Character::~Character()
	{	
		auto* mgr = doCast<PhysicsManagerService>(m_app->getService("physics_mgr"));
		if (!mgr) return;
		PhysicsService* phy_srv = mgr->getPhysicsService();		
		if (phy_srv)
			phy_srv->unregisterEventHandler("pre_physics", this);	
	}

	bool Character::doPhysics(Event* ev, Unit* unit)
	{
		if (!m_physics_body)
			return false;
		
		if (!m_position)
			m_position = getTransform()->get_position();

		float delta = static_cast<PhysicsEvent*>(ev)->getDelta();		

		if (m_move_state != MoveState::GROUND) {
			m_air_timer += delta;
		}
		else {
			m_air_timer = 0.0f;
		}

		m_physics_body->setAngularVelocity(Vector3(0.0, 0.0, 0.0));

		if (m_movement != math::Vector3::ZERO) {
			// currently moving	
			math::Vector3 move;

			// depending on the state we use the players movement or the enforced movement
			if (m_move_state == MoveState::GROUND || m_move_state == MoveState::FLY)
				move = m_movement;
			else
				move = m_forced_movement;
			move *= delta;

			// use physics for movement
			math::Quaternion rot = m_position->get_rotation();
			m_physics_body->applyImpulse(rot * move);

		}

		// gravity 

		// turning

		if (m_turn != 0) {
			float turn_step = m_turn * delta;
			math::Quaternion q;
			q.fromAngleAxis(turn_step, math::Vector3(0, 1, 0));
			math::Quaternion m_orientation = q * m_position->get_rotation(); //* m_orientation;
			m_orientation.normalize();
			//m_position->setOrientation(m_orientation);

			//m_physics_body->setOrientation(m_orientation);
			m_physics_body->applyOrientation(m_orientation);
		}
		
		return false;		
	}

	void Character::ready()
	{
		Item::ready();

		// set defaults
		m_values["character"] = true;

		// register to physics step
		auto* mgr = doCast<PhysicsManagerService>(m_app->getService("physics_mgr"));
		if (!mgr) return;
		PhysicsService* phy_srv = mgr->getPhysicsService();
		if (phy_srv) {
			m_phy_pre_handler = std::bind(&Character::doPhysics, this, std::placeholders::_1, std::placeholders::_2);
			phy_srv->registerEventHandler("pre_physics", this, m_phy_pre_handler);
			m_gravity = getValue("gravity");
		}
		else
			m_parent->registerEventHandler("ready", this, std::bind(&Character::onSceneReady, this, std::placeholders::_1, std::placeholders::_2));
	}

	bool Character::onSceneReady(Event* ev, Unit* unit)
	{
		auto* mgr = doCast<PhysicsManagerService>(m_app->getService("physics_mgr"));
		if (!mgr) return true;
		PhysicsService* phy_srv = mgr->getPhysicsService();
		if (phy_srv) {
			m_phy_pre_handler = std::bind(&Character::doPhysics, this, std::placeholders::_1, std::placeholders::_2);
			phy_srv->registerEventHandler("pre_physics", this, m_phy_pre_handler);
		}
		m_gravity = getValue("gravity");
		return false;
	}
}
