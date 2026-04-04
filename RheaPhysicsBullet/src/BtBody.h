#ifndef RHEA_PHYSICS_BULLET_BODY_H
#define RHEA_PHYSICS_BULLET_BODY_H

#include <Rhea/Core/Position.h>

#include <Rhea/Physics/PhysicsBody.h>
#include <Rhea/Physics/PhysicsBodyDescriptor.h>

class btRigidBody;
class btDynamicsWorld;
class btCompoundShape;

namespace Rhea
{
	class Item;
}

namespace rhea::physics::bullet
{

	class BulletPhysicsBody : public Rhea::PhysicsBody
	{
	public:
		BulletPhysicsBody(btDynamicsWorld* world, Rhea::Item* item, const Rhea::PhysicsBodyDesc& desc);
		~BulletPhysicsBody();

		virtual bool init();

		// move by vector
		void move(Math::Vector3 move) override;

		// turn by radian angle
		void turn(float turn) override;

		// stop the current movement
		void stop() override;

		void setPosition(const Math::Vector3& position) override;
		void setOrientation(const Math::Quaternion& rot) override;

		// functions for setting attributes
		void setAngularVelocity(const Math::Vector3& velocity) override;

		void DisableMassUpdate() override;
		void EnableMassUpdate() override;

		void applyImpulse(const Math::Vector3& impulse) override;
		void applyOrientation(const Math::Quaternion& rot) override;

		void activate();

		void setFriction(float friction);
		
		btRigidBody* getBody() { return m_body; }		
		
	protected:
		Math::Vector3 m_movement;
		float m_turn;
		btRigidBody* m_body;
		btCompoundShape* m_comound_shape;
		float m_upwards;
		btDynamicsWorld* m_world;
		float m_mass;
	};
}

#endif // !RHEA_PHYSICS_BULLET_BODY_H