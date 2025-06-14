#ifndef RHEA_PHYSICS_BULLET_BODY_H
#define RHEA_PHYSICS_BULLET_BODY_H

#include <Rhea/Core/Position.h>

#include "../Body.h"

class btRigidBody;
class btDynamicsWorld;
class btCompoundShape;

namespace Rhea
{
	class Item;

	class BulletPhysicsBody : public PhysicsBody
	{
	public:
		BulletPhysicsBody(btDynamicsWorld* world, Item* item);
		~BulletPhysicsBody();

		virtual bool init();

		// stop the current movement
		void stop();

		void setPosition(Math::Vector3& position);
		void setOrientation(Math::Quaternion& rot);

		// functions for setting attributes
		void setAngularVelocity(const Math::Vector3& velocity);

		void DisableMassUpdate();
		void EnableMassUpdate();

		void applyImpulse(const Math::Vector3& impulse);
		void applyOrientation(Math::Quaternion& rot);

		void activate();

		void setFriction(float friction);
		
		btRigidBody* getBody() { return m_body; }
		
	protected:
		btRigidBody* m_body;
		btCompoundShape* m_comound_shape;
		float m_upwards;
		btDynamicsWorld* m_world;
		float m_mass;
		Item* m_item;
	};
}

#endif // !RHEA_PHYSICS_BULLET_BODY_H