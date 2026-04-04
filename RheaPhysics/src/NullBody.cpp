#include "NullBody.h"

namespace rhea::physics
{
	NullPhysicsBody::NullPhysicsBody(rhea::Item* item) :
		m_turn(0.0f),		
		m_isKinematic(false),
		m_isStatic(false),
		rhea::PhysicsBody(item)
	{
	}

	NullPhysicsBody::~NullPhysicsBody()
	{
	}

	void NullPhysicsBody::move(Math::Vector3 move)
	{
		if (move == Math::Vector3::ZERO &&
			m_movement != Math::Vector3::ZERO) {
			stop();
		}
		m_movement = move;
	}

	void NullPhysicsBody::turn(float turn)
	{
		m_turn = turn;
	}

	void NullPhysicsBody::setPosition(const Math::Vector3& position)
	{

	}


}