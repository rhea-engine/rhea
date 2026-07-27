/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Math/Quaternion.h>

#include "BtGhost.h"
#include <Rhea/Core/Item.h>
#include <Rhea/Components/PhysicsDataComponent.h>

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

using math::Vector3;

namespace rhea::physics::bullet {

	BulletPhysicsGhost::BulletPhysicsGhost(btDynamicsWorld* world, rhea::Item* item, float radius, float height, float offset_y) :
		m_offset_y(offset_y),
		m_radius(radius),
		m_height(height),
		m_ghost_object(nullptr),
		BulletPhysicsBody(world, item)
	{
	}

	BulletPhysicsGhost::~BulletPhysicsGhost()
	{
		if (m_ghost_object) {
			m_world->removeCollisionObject(m_ghost_object);
			delete m_ghost_object;
		}
	}

	bool BulletPhysicsGhost::init()
	{
		// get position and rotation from m_item
		math::Vector3 pos = m_item->getTransform()->getWorldPosition().getTranslation();
		math::Quaternion rot = m_item->getTransform()->getWorldPosition().getRotation();
		if (m_offset_y)
			pos.y += m_offset_y;

		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
		startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

		// create a capsule shape for the ghost object - radius is the radius of the capsule, height is the total height of the capsule (including the hemispherical ends)
        btCollisionShape* capsule = createCollisionShape(m_item->getComponent<PhysicsDataComponent>());

        m_ghost_object = new btPairCachingGhostObject();
        m_ghost_object->setWorldTransform(startTransform);
        m_world->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
        m_ghost_object->setCollisionShape(capsule);
        m_ghost_object->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
		m_ghost_object->setUserPointer((void*)m_item);
		m_world->addCollisionObject(m_ghost_object);
		return true;
	}

	void BulletPhysicsGhost::setPosition(const math::Vector3& position)
	{
		if (m_ghost_object) {
			btTransform newTrans = m_ghost_object->getWorldTransform();
			newTrans.setOrigin(btVector3(position.x, position.y + m_offset_y, position.z));
			m_ghost_object->setWorldTransform(newTrans);
		}
	}
	
	void BulletPhysicsGhost::setOrientation(const math::Quaternion& rot)
	{
		if (m_ghost_object) {
			btTransform newTrans = m_ghost_object->getWorldTransform();
			newTrans.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
			m_ghost_object->setWorldTransform(newTrans);
		}
	}
}