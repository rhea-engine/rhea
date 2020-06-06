#include <Poseidon/Math/Quaternion.h>
#include <Poseidon/Node.h>

#include "BtGhost.h"
#include "Item.h"

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

using Poseidon::Vector3;

namespace Rhea {

	BulletPhysicsGhost::BulletPhysicsGhost(btDynamicsWorld* world, Item* item) : 
		m_offset_y(0.0f),
		m_ghost_object(nullptr),
		BulletPhysicsBody(world, item)
	{
	}

	BulletPhysicsGhost::~BulletPhysicsGhost()
	{
		if (m_ghost_object)
			delete m_ghost_object;
	}

	void BulletPhysicsGhost::init()
	{
		// get position and rotation from m_item
		Poseidon::Vector3 pos = m_item->getNode()->get_position()->get_worldspace();
		Poseidon::Quaternion rot = m_item->getNode()->get_position()->get_rotation();
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
	}

	void BulletPhysicsGhost::setPosition(Poseidon::Vector3& position)
	{
		if (m_ghost_object) {
			btTransform newTrans = m_ghost_object->getWorldTransform();
			newTrans.setOrigin(btVector3(position.x, position.y + m_offset_y, position.z));
			m_ghost_object->setWorldTransform(newTrans);
		}
	}
	
	void BulletPhysicsGhost::setOrientation(Poseidon::Quaternion& rot)
	{
		if (m_ghost_object) {
			btTransform newTrans = m_ghost_object->getWorldTransform();
			newTrans.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
			m_ghost_object->setWorldTransform(newTrans);
		}
	}
}