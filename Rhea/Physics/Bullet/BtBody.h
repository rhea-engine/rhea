#ifndef RHEA_PHYSICS_BULLET_BODY_H
#define RHEA_PHYSICS_BULLET_BODY_H

#include <Poseidon/Position.h>

#include "../Body.h"

class btRigidBody;
class btDynamicsWorld;
class btCompoundShape;

namespace Rhea
{
	class Item;
	class Terrain;

	class BulletPhysicsBody : public PhysicsBody
	{
	public:
		BulletPhysicsBody(btDynamicsWorld* world, Item* item);
		~BulletPhysicsBody();

		void init();

		void doPhysics(float delta, Terrain* terrain);

		// stop the current movement
		void stop();

		void setPosition(Poseidon::Vector3& position);
		void setOrientation(Poseidon::Quaternion& rot);

		// functions for setting attributes
		void setAngularVelocity(const Poseidon::Vector3& velocity);

		void DisableMassUpdate();
		void EnableMassUpdate();

		void applyImpulse(const Poseidon::Vector3& impulse);
		void applyOrientation(Poseidon::Quaternion& rot);

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