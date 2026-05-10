/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_PHYSICS_BULLET_GHOST_H
#define RHEA_PHYSICS_BULLET_GHOST_H

#include "BtBody.h"

class btPairCachingGhostObject;

namespace rhea::physics::bullet
{
	class BulletPhysicsGhost : public BulletPhysicsBody
	{
	public:
		BulletPhysicsGhost(btDynamicsWorld* world, rhea::Item* item, float radius, float height, float offset_y);
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
		float m_radius;
		float m_height;
	};
}

#endif // !RHEA_PHYSICS_BULLET_GHOST_H
