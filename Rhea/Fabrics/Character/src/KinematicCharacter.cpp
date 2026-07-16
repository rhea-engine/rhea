/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <functional>

#include <Rhea/Character/KinematicCharacter.h>
#include <Rhea/Application.h>
#include <Rhea/Service.h>
#include <Rhea/Physics/PhysicsBody.h>
#include <Rhea/Physics/PhysicsEvent.h>
#include <Rhea/Physics/PhysicsManagerService.h>
#include <Rhea/Physics/PhysicsService.h>
#include <Rhea/Physics/IPhysicsCharacter.h>
#include <Rhea/Core/Transform.h>

#include <algorithm>
#include <Psio/Log.h>
#include <Rhea/Components/PhysicsDataComponent.h>

using math::Quaternion;
using math::Vector3;

namespace rhea
{
	KinematicCharacter::~KinematicCharacter()
	{
		auto* mgr = doCast<PhysicsManagerService>(m_app->getService("physics_mgr"));
		if (!mgr) return;
		PhysicsService* phy_srv = mgr->getPhysicsService();		
		if (phy_srv)
			phy_srv->unregisterEventHandler("pre_physics", this);
	}

	void KinematicCharacter::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Item::setup(app, attributes, values);

		// find or create physics data component
		
		// find by type from children
		PhysicsDataComponent* pc = nullptr;
		for (auto child : m_children) {
			if (doCast<PhysicsDataComponent>(child.second)) {
				pc = doCast<PhysicsDataComponent>(child.second);
				break;
			}
		}
		if (!pc)
			pc = addComponentUnique<PhysicsDataComponent>("physics_data");
		pc->rigid_body.motion = RigidBody::MotionType::Kinematic;
		pc->rigid_body.velocity = { 0,0,0 };
		pc->rigid_body.friction = 0.0f;
		pc->rigid_body.kind = "RheaPhysics:Character";
	}

	bool KinematicCharacter::doPhysics(Event* ev, Unit* unit)
	{
		auto* physics_body = getPhysicsBody();
		if (!physics_body)
			return false;
		
		Position position = getTransform()->getWorldPosition();		

		float delta = static_cast<PhysicsEvent*>(ev)->getDelta();		
		auto* character = physics_body->query<IPhysicsCharacter>("PhysicsCharacter");
		if (!character) {
			return false;
		}
		bool dirty = false;

		// if not on ground increase air timer else reset it
		if (m_move_state != MoveState::GROUND) {
			m_air_timer += delta;
		}
		else {
			m_air_timer = 0.0f;
		}

		// handle movement except if if currently standing still on the ground
		if (m_move_state != MoveState::GROUND || m_movement != math::Vector3::ZERO) {
			math::Vector3 move;

			// depending on the state we use the players movement or the enforced movement
			if (m_move_state == MoveState::GROUND || m_move_state == MoveState::FLY)
				move = m_movement;
			else
				move = m_forced_movement;
			
			// move by timestep
			move *= delta;

			// move object itself

			// compute gravity
			m_upwards = m_gravity * (m_air_timer + delta);			

			// handle jumping
			if (m_move_state == MoveState::JUMP) {
				// add jumping
				m_upwards += m_vertical_velocity;

				// if still going up use only delta
				float vertical_offset = m_upwards * delta;

				//if (vertical_offset > 0.0f)
				m_upwards = vertical_offset;
			}
			math::Vector3 gravity = Vector3(0, m_upwards, 0);

			// compute the next position
			math::Vector3 pos = character->update(&position, move, gravity);			
			if (!pos.equals(position.getTranslation())) {
				position.setPosition(pos);
				getTransform()->setPosition(position);
				physics_body->setPosition(pos);
				dirty = true;
			}

			// check if we ended up on ground
			if (character->isOnGround()) {
				// yes - reset to ground move
				m_move_state = MoveState::GROUND;
			}
			else if (m_move_state == MoveState::GROUND) {
				// no - if we were on ground we are now falling
				m_move_state = MoveState::FALL;
			}
		
		}

		// turning

		if (m_turn != 0) {
			float turn_step = m_turn * delta;
			math::Quaternion q;
			q.fromAngleAxis(turn_step, math::Vector3(0, 1, 0));
			math::Quaternion m_orientation = q * position.getRotation();
			m_orientation.normalize();
			position.setRotation(m_orientation);
			getTransform()->setWorldPosition(position);
			dirty = true;
			physics_body->setOrientation(m_orientation);
		}
		if (dirty) {
			for (auto* l : m_movement_listeners) {
				l->onTransformChanged();
			}
		}
		return false;
	}

	void KinematicCharacter::jump()
	{
		// only jump from ground
		if (m_move_state == MoveState::GROUND) {
			
			m_air_timer = 0.0f;
			// set the forced movement to the current movement
			m_forced_movement = m_movement;			

			Position position = getTransform()->getWorldPosition();
			math::Vector3 pos = position.getTranslation();
			m_jump_start_y = pos.y;
			auto* physics_body = getPhysicsBody();
			if (!physics_body)
				return;
			auto* character = physics_body->query<IPhysicsCharacter>("PhysicsCharacter");
			if (!character) {
				return;
			}
			if (character->jump(pos)) {
				// jumping is possible
				position.setPosition(pos);
				getTransform()->setWorldPosition(position);
				m_move_state = MoveState::JUMP;
				m_vertical_velocity = 6.0f;
			}
		}
	}

	void KinematicCharacter::ready()
	{
		Item::ready();

		// set some defaults
		m_values["kinematic"] = true;
		m_values["body_type"] = "ghost";
		
		if (m_parent)
			m_gravity =
			getValue("gravity", m_parent->getValue("linear_gravity"));
	}

	void KinematicCharacter::onActivate()
	{
		auto* mgr = doCast<PhysicsManagerService>(m_app->getService("physics_mgr"));
		if (!mgr) return;
		PhysicsService* phy_srv = mgr->getPhysicsService();
		// bind our physics loop
		if (phy_srv) {
			m_phy_pre_handler = std::bind(&KinematicCharacter::doPhysics, this, std::placeholders::_1, std::placeholders::_2);
			phy_srv->registerEventHandler("pre_physics", this, m_phy_pre_handler);
		}
	}

	void KinematicCharacter::onDeactivate()
	{
		auto* mgr = doCast<PhysicsManagerService>(m_app->getService("physics_mgr"));
		if (!mgr) return;
		PhysicsService* phy_srv = mgr->getPhysicsService();
		if (phy_srv)
			phy_srv->unregisterEventHandler("pre_physics", this);
	}
}
