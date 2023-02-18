#ifndef RHEA_PHYSICS_BULLET_GHOST_H
#define RHEA_PHYSICS_BULLET_GHOST_H

#include "BtBody.h"

class btPairCachingGhostObject;

namespace Rhea
{
	class BulletPhysicsGhost : public BulletPhysicsBody
	{
	public:
		BulletPhysicsGhost(btDynamicsWorld* world, Item* item);
		~BulletPhysicsGhost();

		bool init();

		btPairCachingGhostObject* getObject() { return m_ghost_object; }

		// set position
		void setPosition(Math::Vector3& position);

		// set orientation
		void setOrientation(Math::Quaternion& rot);

		// stop the current movement
		void stop() {}

	protected:
		btPairCachingGhostObject* m_ghost_object;
		float m_offset_y;
	};
}

#endif // !RHEA_PHYSICS_BULLET_GHOST_H
