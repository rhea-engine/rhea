#include <Math/Quaternion.h>

#include "BtGhost.h"
#include "Fabrics/Core/Item.h"

#include <bullet/src/btBulletDynamicsCommon.h>
#include <bullet/src/BulletCollision/CollisionDispatch/btGhostObject.h>

using Math::Vector3;

namespace Rhea {

	BulletPhysicsGhost::BulletPhysicsGhost(btDynamicsWorld* world, Item* item) : 
		m_offset_y(0.0f),
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
		Math::Vector3 pos = m_item->getTransform()->get_position()->get_worldspace();
		Math::Quaternion rot = m_item->getTransform()->get_position()->get_rotation();
		m_offset_y = m_item->getValue("collision_shape_offset_y");
		if (m_offset_y)
			pos.y += m_offset_y;

		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
		startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

		float radius = m_item->getValue("collision_shape_radius", 0.7f);
		float height = m_item->getValue("collision_shape_height", 1.8f);

        btConvexShape* capsule = new btCapsuleShape(radius, height - radius);
        
        m_ghost_object = new btPairCachingGhostObject();
        m_ghost_object->setWorldTransform(startTransform);
        m_world->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
        m_ghost_object->setCollisionShape(capsule);
        m_ghost_object->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
		m_world->addCollisionObject(m_ghost_object);
		return true;
	}

	void BulletPhysicsGhost::setPosition(Math::Vector3& position)
	{
		if (m_ghost_object) {
			btTransform newTrans = m_ghost_object->getWorldTransform();
			newTrans.setOrigin(btVector3(position.x, position.y + m_offset_y, position.z));
			m_ghost_object->setWorldTransform(newTrans);
		}
	}
	
	void BulletPhysicsGhost::setOrientation(Math::Quaternion& rot)
	{
		if (m_ghost_object) {
			btTransform newTrans = m_ghost_object->getWorldTransform();
			newTrans.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
			m_ghost_object->setWorldTransform(newTrans);
		}
	}
}