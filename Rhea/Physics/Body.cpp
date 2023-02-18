#include "Body.h"
#include "Body.h"

namespace Rhea
{
	PhysicsBody::PhysicsBody(Item* item) :
		m_turn(0.0f),		
		m_isKinematic(false),
		m_isStatic(false)
	{
		m_item = item;
	}

	PhysicsBody::~PhysicsBody()
	{
	}

	void PhysicsBody::move(Math::Vector3 move)
	{
		if (move == Math::Vector3::ZERO &&
			m_movement != Math::Vector3::ZERO) {
			stop();
		}
		m_movement = move;
	}

	void PhysicsBody::turn(float turn)
	{
		m_turn = turn;
	}

	void PhysicsBody::setPosition(Math::Vector3& position)
	{

	}


}