#ifndef RHEA_PHYSICS_BULLET_GHOST_H
#define RHEA_PHYSICS_BULLET_GHOST_H

#include "BtBody.h"

class btPairCachingGhostObject;

namespace rhea::physics::bullet
{
	class BulletPhysicsGhost : public BulletPhysicsBody
	{
	public:
		BulletPhysicsGhost(btDynamicsWorld* world, rhea::Item* item, const rhea::PhysicsBodyDesc& desc);
		~BulletPhysicsGhost();

		bool init();

		btPairCachingGhostObject* getObject() { return m_ghost_object; }

		// set position
		void setPosition(const math::Vector3& position) override;

		// set orientation
		void setOrientation(const math::Quaternion& rot) override;

		// stop the current movement
		void stop() {}

	protected:
		btPairCachingGhostObject* m_ghost_object;
		float m_offset_y;
	};
}

#endif // !RHEA_PHYSICS_BULLET_GHOST_H
