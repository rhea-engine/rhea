#include "BtBody.h"
#include "../Item.h"
#include <Poseidon/Node.h>

#include "btBulletDynamicsCommon.h"

namespace Rhea 
{
	BulletPhysicsBody::BulletPhysicsBody(Item* item) : 
		PhysicsBody(item)
	{
		//create a dynamic rigidbody

		btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		//btCollisionShape* colShape = new btSphereShape(btScalar(1.));
//TODO (for cleanup and reuse)		collisionShapes.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(item->getValue("mass"));

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		// get positionand rotation from item
		Poseidon::Vector3 pos = item->getNode()->get_position()->get_worldspace();
		Poseidon::Quaternion rot = item->getNode()->get_position()->get_rotation();
		startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
		startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		m_body = new btRigidBody(rbInfo);
		m_body->setUserPointer((void*)item);
	}
	
	BulletPhysicsBody::~BulletPhysicsBody()
	{
		if (m_body)
			delete m_body;
	}
	
	void BulletPhysicsBody::doPhysics(float delta)
	{
		if (m_movement != Poseidon::Vector3::ZERO) {
			// currently moving			
			Poseidon::Vector3 move = m_movement;
			move *= delta * 10;
			Poseidon::Quaternion rot = m_item->getNode()->get_position()->get_rotation();

			Poseidon::Vector3 t = rot * move;

			m_body->setLinearVelocity(btVector3(t.x, t.y, t.z));
			m_body->setActivationState(ACTIVE_TAG);
		}		
		if (m_turn != 0) {
			float turn_step = m_turn * delta;

			btQuaternion q = btQuaternion(btVector3(0, 1, 0), turn_step);

			btTransform trans = m_body->getWorldTransform();
			btQuaternion rot = trans.getRotation();

			btQuaternion orientation = q * rot;
			orientation.normalize();
			
			trans.setRotation(orientation);
			m_body->setWorldTransform(trans);
			m_body->setActivationState(ACTIVE_TAG);
		}
	}
	
	void BulletPhysicsBody::stop()
	{
		m_body->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
		m_body->setActivationState(ACTIVE_TAG);
	}

}
