#include "Body.h"

namespace Rhea
{
	PhysicsBody::PhysicsBody(Item* item) :
		m_turn(0.0f)
	{
		m_item = item;
	}

	PhysicsBody::~PhysicsBody()
	{
	}

	void PhysicsBody::move(Poseidon::Vector3 move)
	{
		if (move == Poseidon::Vector3::ZERO &&
			m_movement != Poseidon::Vector3::ZERO) {
			stop();
		}
		m_movement = move;
	}

	void PhysicsBody::turn(float turn)
	{
		m_turn = turn;
	}


}