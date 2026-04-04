#ifndef RHEA_NULLPHYSICS_BODY_H
#define RHEA_NULLPHYSICS_BODY_H

#include <Rhea/Core/Position.h>

#include <Rhea/Element.h>
#include <string>
#include <Rhea/Physics/PhysicsBody.h>

namespace
{
	class Item;
}

namespace rhea::physics
{
	class NullPhysicsBody : public Rhea::PhysicsBody
	{
	public:
		NullPhysicsBody(Rhea::Item* item);
		virtual ~NullPhysicsBody();

		// move by vector
		void move(Math::Vector3 move);

		// turn by radian angle
		void turn(float turn);

		// set position
		virtual void setPosition(const Math::Vector3& position) override;

		// set orientation
		virtual void setOrientation(Math::Quaternion& rot) {}
		virtual void applyOrientation(Math::Quaternion& rot) {}

		// stop the current movement
		virtual void stop() {};

		virtual void DisableMassUpdate() {}
		virtual void EnableMassUpdate() {}
		virtual void applyImpulse(const Math::Vector3& impulse) {}
		virtual void setAngularVelocity(const Math::Vector3& velocity) {}

		// can we controll the move?
		virtual bool isMoveControllable() { return true; }

		// is it a kinematic object 
		bool isKinematic() { return m_isKinematic; }

		// is it a dynamic object
		bool isDynamic() { return !m_isKinematic && !m_isStatic; }		

	protected:		
		Math::Vector3 m_movement;
		float m_turn;
		bool m_isKinematic;
		bool m_isStatic;
	};
}

#endif // !RHEA_NULLPHYSICS_BODY_H