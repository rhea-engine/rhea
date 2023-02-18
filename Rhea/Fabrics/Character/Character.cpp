#include "Character.h"
#include <functional>

#include "Character.h"
#include "Application.h"
#include "Service.h"
#include "Physics/Body.h"
#include "Physics/PhysicsEvent.h"
#include "Physics/PhysicsManager.h"
#include "Fabrics/Terrain/Terrain.h"

using Math::Quaternion;
using Math::Vector3;

namespace Rhea
{
	Character::~Character()
	{		
		Service* phy_srv = m_app->getService("physics");
		phy_srv->unregisterEventHandler("pre_physics", this);	
	}

	bool Character::doPhysics(Event* ev, Unit* unit)
	{
		if (!m_physics_body)
			if (!getPhysicsBody())
				return false;
		
		if (!m_position)
			m_position = getTransform()->get_position();

		float delta = static_cast<PhysicsEvent*>(ev)->getDelta();
		Terrain* terrain = static_cast<PhysicsEvent*>(ev)->getTerrain();

		if (m_move_state != MoveState::GROUND) {
			m_air_timer += delta;
		}
		else {
			m_air_timer = 0.0f;
		}

		m_physics_body->setAngularVelocity(Vector3(0.0, 0.0, 0.0));

		if (m_movement != Math::Vector3::ZERO) {
			// currently moving	
			Math::Vector3 move;

			// depending on the state we use the players movement or the enforced movement
			if (m_move_state == MoveState::GROUND || m_move_state == MoveState::FLY)
				move = m_movement;
			else
				move = m_forced_movement;
			move *= delta;

			// use physics for movement
			Math::Quaternion rot = m_position->get_rotation();
			m_physics_body->applyImpulse(rot * move);

			// move object itself

			/*
			Math::Vector3 pos = m_position->moveRelative(move);

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
			Math::Vector3 pos = m_position->moveRelative(Math::Vector3(0.0f, m_upwards, 0.0f));

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
			Math::Quaternion q;
			q.fromAngleAxis(turn_step, Math::Vector3(0, 1, 0));
			Math::Quaternion m_orientation = q * m_position->get_rotation(); //* m_orientation;
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
		Service* phy_srv = m_app->getService("physics");
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
		Service* phy_srv = m_app->getService("physics");
		if (phy_srv) {
			m_phy_pre_handler = std::bind(&Character::doPhysics, this, std::placeholders::_1, std::placeholders::_2);
			phy_srv->registerEventHandler("pre_physics", this, m_phy_pre_handler);
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
