#ifndef RHEA_PHYSICS_BODY_H
#define RHEA_PHYSICS_BODY_H

#include <Poseidon/Position.h>

#include "Element.h"

namespace Rhea
{
	class Item;

	class PhysicsBody : public Element
	{
	public:
		PhysicsBody(Item* item);
		virtual ~PhysicsBody();

		// move by vector
		void move(Poseidon::Vector3 move);

		// turn by radian angle
		void turn(float turn);

		// set position
		virtual void setPosition(Poseidon::Vector3& position);

		// set orientation
		virtual void setOrientation(Poseidon::Quaternion& rot) {}
		virtual void applyOrientation(Poseidon::Quaternion& rot) {}

		// stop the current movement
		virtual void stop() {};

		virtual void DisableMassUpdate() {}
		virtual void EnableMassUpdate() {}
		virtual void applyImpulse(const Poseidon::Vector3& impulse) {}
		virtual void setAngularVelocity(const Poseidon::Vector3& velocity) {}

		// can we controll the move?
		virtual bool isMoveControllable() { return true; }

		// is it a kinematic object 
		bool isKinematic() { return m_isKinematic; }

		// is it a dynamic object
		bool isDynamic() { return !m_isKinematic && !m_isStatic; }

	protected:		
		Poseidon::Vector3 m_movement;
		float m_turn;
		Item* m_item;
		bool m_isKinematic;
		bool m_isStatic;
	};
}

#endif // !RHEA_PHYSICS_BODY_H