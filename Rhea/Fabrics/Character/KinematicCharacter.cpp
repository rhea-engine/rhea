#include <functional>

#include "KinematicCharacter.h"
#include "Application.h"
#include "Service.h"
#include "Physics/Body.h"
#include "Physics/PhysicsEvent.h"
#include "Physics/PhysicsManager.h"
#include "Fabrics/Terrain/Terrain.h"
#include "Physics/Bullet/BtGhost.h"
#include "Physics/PhysicsCharacter.h"
#include <algorithm>

using Poseidon::Quaternion;
using Poseidon::Vector3;

namespace Rhea
{
	bool KinematicCharacter::doPhysics(Event* ev, Unit* unit)
	{
		if (!m_physics_body)
			if (!getPhysicsBody())
				return false;

		if (!m_position) {
			Poseidon::Node* node = getNode();
			if (!node)
				return false;
			m_position = node->get_position();
		}

		float delta = static_cast<PhysicsEvent*>(ev)->getDelta();
		Terrain* terrain = static_cast<PhysicsEvent*>(ev)->getTerrain();
		PhysicsCharacter* character = static_cast<PhysicsCharacter*>(m_physics_body);

		// if not on ground increase air timer else reset it
		if (m_move_state != MoveState::GROUND) {
			m_air_timer += delta;
		}
		else {
			m_air_timer = 0.0f;
		}

		// handle movement except if if currently standing still on the ground
		if (m_move_state != MoveState::GROUND || m_movement != Poseidon::Vector3::ZERO) {
			Poseidon::Vector3 move;

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
			Poseidon::Vector3 gravity = Vector3(0, m_upwards, 0);

			// compute the next possitions
			Poseidon::Vector3 pos = character->update(m_position, move, gravity);
			m_position->setPosition(pos);
			character->setPosition(pos);

			// check if we ended up on ground
			if (character->isOnGround()) {
				// yes - reset to ground move
				m_move_state = MoveState::GROUND;
			}
			else if (m_move_state == MoveState::GROUND) {
				// no - if we were on ground we are now falling
				m_move_state = MoveState::FALL;
			}

			/*if (terrain && m_gravity) {
				float height = terrain->getHeight(pos.x, pos.z);
				if (pos.y < height) {
					pos.y = height;
					m_position->setPosition(pos);
				}
				
			}
			*/
		}

		// turning

		if (m_turn != 0) {
			float turn_step = m_turn * delta;
			Poseidon::Quaternion q;
			q.fromAngleAxis(turn_step, Poseidon::Vector3(0, 1, 0));
			Poseidon::Quaternion m_orientation = q * m_position->get_rotation();
			m_orientation.normalise();
			m_position->setOrientation(m_orientation);

			m_physics_body->setOrientation(m_orientation);
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

			Poseidon::Vector3 pos = m_position->get_worldspace();
			m_jump_start_y = pos.y;
			if (static_cast<PhysicsCharacter*>(m_physics_body)->jump(pos)) {
				// jumping is possible
				m_position->setPosition(pos);
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

		// register to physics step
		m_parent->registerEventHandler("ready", std::bind(&KinematicCharacter::onSceneReady, this, std::placeholders::_1, std::placeholders::_2));
	}

	bool KinematicCharacter::onSceneReady(Event* ev, Unit* unit)
	{
		Service* phy_srv = m_app->getService("physics");
		// bind our physics loop
		if (phy_srv) {
			phy_srv->registerEventHandler("pre_physics", std::bind(&KinematicCharacter::doPhysics, this, std::placeholders::_1, std::placeholders::_2));
		}
		// gravity - default is scene but char may overwrite it
		m_gravity = getValue("gravity", unit->getValue("gravity"));
		
		return false;
	}

	Unit* KinematicCharacter::createType(std::string name, std::string type)
	{
		KinematicCharacter* character = new KinematicCharacter(name, type);
		return character;
	}
}
