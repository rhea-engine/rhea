#include "BtBody.h"
#include "Fabrics/3D/Item.h"
#include "Fabrics/3D/Mesh.h"
#include "Fabrics/Terrain/Terrain.h"
#include <Psio/Log.h>

#include <bullet/src/btBulletDynamicsCommon.h>

using Math::Vector3;

namespace Rhea 
{
	BulletPhysicsBody::BulletPhysicsBody(btDynamicsWorld* world, Item* item) :
		m_world(world),
		m_item(item),
		m_comound_shape(nullptr),		
		m_mass(0.0f),
		m_upwards(0.0f),
		m_body(nullptr),
		PhysicsBody(item)
	{}
	
	bool BulletPhysicsBody::init()
	{	
		//create a rigidbody

		m_mass = m_item->getValue("mass");
		m_isKinematic = m_item->getValue("kinematic");
		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (m_mass != 0.f);

		// get the info from item 
		std::string shape_type = m_item->getValue("collision_shape");
		btCollisionShape* colShape;
		if (shape_type == "Capsule") {
			float radius = m_item->getValue("collission_shape.radius");
			if (!radius)
				radius = 0.7f;
			float height = m_item->getValue("collission_shape.height");
			if (!height)
				height = 1.8f;
			colShape = new btCapsuleShape(radius, height);
		}
		else if (shape_type == "Sphere") {
			float radius = m_item->getValue("collission_shape.radius");
			if (!radius)
				radius = 1.0f;
			colShape = new btSphereShape(btScalar(radius));
		}
		else {
			// default

			if (!isDynamic && !m_isKinematic) {
				// for static object we use the triangles
				Mesh* mesh = m_item->getMesh();
				if (!mesh) {
					return false;
				}
				VertexBuffer* vb = mesh->getVertexBuffer();
				if (vb && vb->size()) {
					std::vector<unsigned long> indices = mesh->getIndices();
					btTriangleMesh* triMesh = new btTriangleMesh();
					for (size_t i = 0; i < indices.size(); i += 3) {
						size_t j = indices[i];
						Math::Vector3 vertex = (Math::Vector3)(*vb)[j];
						btVector3 v1 = btVector3(vertex.x, vertex.y, vertex.z);
						j = indices[i + 1];
						vertex = (Math::Vector3)(*vb)[j];
						btVector3 v2 = btVector3(vertex.x, vertex.y, vertex.z);
						j = indices[i + 2];
						vertex = (Math::Vector3)(*vb)[j];
						btVector3 v3 = btVector3(vertex.x, vertex.y, vertex.z);
						triMesh->addTriangle(v1, v2, v3);
					}

					if (!indices.size()) {
						LOG(LOG_ERROR) << "BtBody: no indices for item";
						colShape = new btBoxShape(btVector3(1, 1, 1));
					}
					else {
						colShape = new btBvhTriangleMeshShape(triMesh, true);
					}
				}
				else {
					std::vector<Mesh::vertex> vertices;
					std::vector<unsigned long> indices;
					mesh->getBuffers(vertices, indices);

					btTriangleMesh* triMesh = new btTriangleMesh();
					for (int i = 0; i < indices.size(); i += 3) {
						int j = indices[i];
						btVector3 v1 = btVector3(vertices[j].x, vertices[j].y, vertices[j].z);
						j = indices[i + 1];
						btVector3 v2 = btVector3(vertices[j].x, vertices[j].y, vertices[j].z);
						j = indices[i + 2];
						btVector3 v3 = btVector3(vertices[j].x, vertices[j].y, vertices[j].z);
						triMesh->addTriangle(v1, v2, v3);
					}

					if (!indices.size()) {
						LOG(LOG_ERROR) << "BtBody: no indices for item";
						colShape = new btBoxShape(btVector3(1, 1, 1));
					}
					else {
						colShape = new btBvhTriangleMeshShape(triMesh, true);
					}
				}
			}
			else {
				// for dynamic objects we handle it not correctly...
				colShape = new btBoxShape(btVector3(1, 1, 1));
			}
		}
	
//TODO (for cleanup and reuse)		collisionShapes.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();
		
		m_upwards = 0.0f;

		btVector3 localInertia(0, 0, 0);
		if (isDynamic&& !m_isKinematic)
			colShape->calculateLocalInertia(m_mass, localInertia);

		// get position and rotation from m_item
		Transform* transform = m_item->getTransform();
		Math::Vector3 pos = transform->get_position()->get_worldspace();
		Math::Quaternion rot = transform->get_position()->get_rotation();
		float offset_y = m_item->getValue("collision_shape_offset_y");
		if (offset_y)
			pos.y += offset_y;
		startTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
		startTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(m_mass, myMotionState, colShape, localInertia);
		m_body = new btRigidBody(rbInfo);

		if (m_isKinematic) {
			m_body->setCollisionFlags(m_body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			m_body->setActivationState(DISABLE_DEACTIVATION);
		}
		m_body->setUserPointer((void*)m_item);

		if (m_item->getValue("character")) {
			setAngularVelocity(Vector3(0.0, 0.0, 0.0));
			setFriction(0.0f);
		}
		return true;
	}
	
	BulletPhysicsBody::~BulletPhysicsBody()
	{
		if (m_body) {
			auto motionState = m_body->getMotionState();
			if (motionState) delete motionState;
			m_world->removeRigidBody(m_body);
			delete m_body;
			m_body = nullptr;
		}
	}

	void BulletPhysicsBody::doPhysics(float delta, Terrain *terrain)
	{		
		if (m_movement != Math::Vector3::ZERO) {
			// currently moving			
			Math::Vector3 move = m_movement;
			move *= delta;
			
			if (m_isKinematic) {
				btTransform transform;
				transform.setIdentity();

				// move object itself
			
				Math::Vector3 pos = m_item->getTransform()->get_position()->moveRelative(move);

				if (terrain) {
					float height = terrain->getHeight(pos.x, pos.z);
					if (pos.y < height) {
						pos.y = height;						
						m_item->getTransform()->get_position()->setPosition(pos);

					}
				}

				transform.setOrigin(btVector3(
					btScalar(pos.x),
					btScalar(pos.y),
					btScalar(pos.z)));

				m_body->getMotionState()->setWorldTransform(transform);
			}
			else {
				Math::Quaternion rot = m_item->getTransform()->get_position()->get_rotation();
				Math::Vector3 t = rot * move;
				m_body->setLinearVelocity(btVector3(t.x, t.y, t.z));
				m_body->setActivationState(ACTIVE_TAG);
			}
		}

		// gravity for kinematic objects
		/*
		if (terrain && m_gravity && m_isKinematic) {
			m_upwards = m_gravity * delta;
			Math::Vector3 pos = m_item->getTransform()->get_position()->moveRelative(Math::Vector3(0.0f, m_upwards, 0.0f));

			float height = terrain->getHeight(pos.x, pos.z);
			if (pos.y < height) {
				pos.y = height;
				m_item->getTransform()->get_position()->setPosition(pos);
			}

			btTransform transform;
			transform.setIdentity();
			transform.setOrigin(btVector3(
				btScalar(pos.x),
				btScalar(pos.y),
				btScalar(pos.z)));

			m_body->getMotionState()->setWorldTransform(transform);
		}
		*/

		if (m_turn != 0) {
			float turn_step = m_turn * delta;
			if (!m_isKinematic) {
				btQuaternion bt_q = btQuaternion(btVector3(0, 1, 0), turn_step);

				btTransform trans = m_body->getWorldTransform();
				btQuaternion rot = trans.getRotation();

				btQuaternion orientation = bt_q * rot;
				orientation.normalize();

				trans.setRotation(orientation);
				m_body->setWorldTransform(trans);

				m_body->setActivationState(ACTIVE_TAG);
			}
			else {
				Math::Quaternion q;
				q.fromAngleAxis(turn_step, Math::Vector3(0, 1, 0));
				Math::Quaternion m_orientation = q * m_item->getTransform()->get_position()->get_rotation(); //* m_orientation;
				m_orientation.normalise();				
				m_item->getTransform()->get_position()->setOrientation(m_orientation);
				
				btQuaternion bt_orientation = btQuaternion(
					btScalar(m_orientation.x),
					btScalar(m_orientation.y),
					btScalar(m_orientation.z),
					btScalar(m_orientation.w));
				btTransform trans;
				trans.setRotation(bt_orientation);
				m_body->setWorldTransform(trans);
			}

		}
	}
	
	void BulletPhysicsBody::stop()
	{
		m_body->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
		m_body->setActivationState(ACTIVE_TAG);
	}

	void BulletPhysicsBody::setPosition(Math::Vector3& position)
	{
		btTransform transform;
		transform.setIdentity();

		transform.setOrigin(btVector3(
			btScalar(position.x),
			btScalar(position.y),
			btScalar(position.z)));

		m_body->getMotionState()->setWorldTransform(transform);
	}

	void BulletPhysicsBody::setOrientation(Math::Quaternion& rot)
	{
		btQuaternion bt_orientation = btQuaternion(
			btScalar(rot.x),
			btScalar(rot.y),
			btScalar(rot.z),
			btScalar(rot.w));
		btTransform trans;
		trans.setRotation(bt_orientation);
		m_body->setWorldTransform(trans);
	}

	void BulletPhysicsBody::setAngularVelocity(const Math::Vector3& velocity)
	{
		if (m_body)
		{
			m_body->setAngularVelocity(btVector3(velocity.x,velocity.y,velocity.z));
			if (velocity != Vector3::ZERO)
				activate();			
		}
	}

	void BulletPhysicsBody::DisableMassUpdate()
	{
		
	}

	void BulletPhysicsBody::EnableMassUpdate()
	{
	}

	void BulletPhysicsBody::applyImpulse(const Vector3& impulse)
	{
		if (m_body && impulse != Vector3::ZERO)
		{
			activate();
			m_body->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
		}
	}

	void BulletPhysicsBody::applyOrientation(Math::Quaternion& rot)
	{
		btTransform trans = m_body->getWorldTransform();
		
		trans.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
		m_body->setWorldTransform(trans);

		m_body->setActivationState(ACTIVE_TAG);
	}

	void BulletPhysicsBody::activate()
	{
		if (m_body && m_mass > 0.0f)
			m_body->activate(true);
	}
	
	void BulletPhysicsBody::setFriction(float friction)
	{
		if (m_body)
		{
			m_body->setFriction(friction);			
		}
	}
}
