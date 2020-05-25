#ifndef RHEA_PHYSICS_BULLET_BODY_H
#define RHEA_PHYSICS_BULLET_BODY_H

#include <Poseidon/Position.h>

#include "../Body.h"

class btRigidBody;

namespace Rhea
{
	class Item;

	class BulletPhysicsBody : public PhysicsBody
	{
	public:
		BulletPhysicsBody(Item* item);
		~BulletPhysicsBody();

		void doPhysics(float delta);

		// stop the current movement
		void stop();

		btRigidBody* getBody() { return m_body; }
	protected:
		btRigidBody* m_body;
	};
}

#endif // !RHEA_PHYSICS_BULLET_BODY_H