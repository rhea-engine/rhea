#include <functional>

#include "Character.h"
#include "Application.h"
#include "Service.h"
#include "Physics/Body.h"
#include "Physics/PhysicsEvent.h"
#include "Physics/PhysicsManager.h"
#include "Fabrics/Terrain/Terrain.h"

using Poseidon::Quaternion;
using Poseidon::Vector3;

namespace Rhea
{
	bool Character::doPhysics(Event* ev, Unit* unit)
	{
		if (!m_physics_body)
			if (!getPhysicsBody())
				return false;
		
		if (!m_position)
			m_position = getNode()->get_position();

		float delta = static_cast<PhysicsEvent*>(ev)->getDelta();
		Terrain* terrain = static_cast<PhysicsEvent*>(ev)->getTerrain();

		if (m_move_state != MoveState::GROUND) {
			m_air_timer += delta;
		}
		else {
			m_air_timer = 0.0f;
		}

		m_physics_body->setAngularVelocity(Vector3(0.0, 0.0, 0.0));

		if (m_movement != Poseidon::Vector3::ZERO) {
			// currently moving	
			Poseidon::Vector3 move;

			// depending on the state we use the players movement or the enforced movement
			if (m_move_state == MoveState::GROUND || m_move_state == MoveState::FLY)
				move = m_movement;
			else
				move = m_forced_movement;
			move *= delta;

			// use physics for movement
			Poseidon::Quaternion rot = m_position->get_rotation();
			m_physics_body->applyImpulse(rot * move);

			// move object itself

			/*
			Poseidon::Vector3 pos = m_position->moveRelative(move);

			if (terrain && m_gravity) {
				float height = terrain->getHeight(pos.x, pos.z);
				if (pos.y < height) {
					pos.y = height;
					m_position->setPosition(pos);
				}
			}

			m_physics_body->setPosition(pos);
			*/
		}

		// gravity 
/*
		if (terrain && m_gravity) {
			m_upwards = m_gravity * delta;
			Poseidon::Vector3 pos = m_position->moveRelative(Poseidon::Vector3(0.0f, m_upwards, 0.0f));

			float height = terrain->getHeight(pos.x, pos.z);
			if (pos.y < height) {
				pos.y = height;
				m_position->setPosition(pos);
			}

			m_physics_body->setPosition(pos);
		}		
*/
		// turning

		if (m_turn != 0) {
			float turn_step = m_turn * delta;
			Poseidon::Quaternion q;
			q.fromAngleAxis(turn_step, Poseidon::Vector3(0, 1, 0));
			Poseidon::Quaternion m_orientation = q * m_position->get_rotation(); //* m_orientation;
			m_orientation.normalise();
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
		m_parent->registerEventHandler("ready", std::bind(&Character::onSceneReady, this, std::placeholders::_1, std::placeholders::_2));
	}

	bool Character::onSceneReady(Event* ev, Unit* unit)
	{
		Service* phy_srv = m_app->getService("physics");
		if (phy_srv) {
			phy_srv->registerEventHandler("pre_physics", std::bind(&Character::doPhysics, this, std::placeholders::_1, std::placeholders::_2));
		}
		m_gravity = getValue("gravity");
		return false;
	}

	Unit* Character::createType(std::string name, std::string type)
	{
		Character* character = new Character(name, type);
		return character;
	}
}
