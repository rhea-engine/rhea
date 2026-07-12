/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "BtBody.h"
#include <Rhea/Core/Item.h>
#include <Rhea/Core/Mesh.h>
#include <Psio/Log.h>

#include <bullet/src/btBulletDynamicsCommon.h>
#include "ObjTriangleLoader.h"
#include <Rhea/Components/PhysicsDataComponent.h>
#include <Rhea/Components/ColliderComponent.h>


using math::Vector3;

namespace rhea::physics::bullet
{
	static inline btVector3 toBt(const math::Vector3& v);
	static inline btQuaternion toBt(const math::Quaternion& q);

	BulletPhysicsBody::BulletPhysicsBody(btDynamicsWorld* world, rhea::Item* item) :
		m_world(world),				
		m_mass(0.0f),
		m_upwards(0.0f),
		m_body(nullptr),
		m_turn(0.0f),
		rhea::PhysicsBody(item)
	{}
	
	bool BulletPhysicsBody::init()
	{
		//create a rigidbody
		auto* pdc = m_item->getComponent<PhysicsDataComponent>();
		if (!pdc) {
			LOG(LOG_ERROR) << "BtBody: no PhysicsDataComponent for item " << m_item->getPath();
			return false;
		}

		m_mass = pdc->rigid_body.mass;
		m_isKinematic = pdc->rigid_body.motion == RigidBody::MotionType::Kinematic;
		m_isStatic = pdc->rigid_body.motion == RigidBody::MotionType::Static;

		// collider		
		btCollisionShape* colShape = createCollisionShape(pdc);
		if (!colShape) {
			LOG(LOG_ERROR) << "BtBody: no collision shape for item " << m_item->getPath();
			return false;
		}		

		// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		m_upwards = 0.0f;

		btVector3 localInertia(0, 0, 0);
		if (isDynamic() && !m_isKinematic)
			colShape->calculateLocalInertia(m_mass, localInertia);

		// get position and rotation from m_item
		rhea::Transform* transform = m_item->getTransform();
		math::Vector3 pos = transform->getWorldPosition().getTranslation();
		math::Quaternion rot = transform->getWorldPosition().getRotation();		
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

		return true;
	}

	btCollisionShape* BulletPhysicsBody::createCollisionShape(PhysicsDataComponent* pdc)
	{
		if (!pdc || pdc->colliders.empty()) {
			return nullptr;
		}

		if (pdc->colliders.size() == 1 && pdc->colliders[0]->offset==Vector3::ZERO) {
			return createColliderShape(pdc->colliders[0]->shape);
		}

		auto* compound = new btCompoundShape();

		for (const auto& collider : pdc->colliders) {
			auto* childShape = createColliderShape(collider->shape);
			if (!childShape)
				continue;

			btTransform localTransform;
			localTransform.setIdentity();			
			localTransform.setOrigin(toBt(collider->offset));
			// TODO
			//localTransform.setRotation(toBt(collider->localRotation));

			compound->addChildShape(localTransform, childShape);
		}

		return compound;
	}

	btCollisionShape* BulletPhysicsBody::createColliderShape(ColliderShape* shape)
	{
		std::string shape_type = shape->getShapeType();

		// get the info from items physicsdatacomponent		
		btCollisionShape* colShape;
		if (shape_type == "Capsule") {
			rhea::CapsuleColliderShape* capsuleShape = static_cast<rhea::CapsuleColliderShape*>(shape);
			float radius = capsuleShape->radius;
			if (radius < 0.0005f)
				radius = 0.7f;
			// total height of the capsule is height + 2 * radius, so we need to make sure height is not too small
			float height = capsuleShape->height;
			if (height - radius * 2.0f < 0.0005f)
				height = 1.8f;
			colShape = new btCapsuleShape(radius, height - radius * 2.0f);
		}
		else if (shape_type == "Sphere") {
			rhea::SphereColliderShape* sphereShape = static_cast<rhea::SphereColliderShape*>(shape);
			float radius = sphereShape->radius;
			if (radius < 0.0005f)
				radius = 1.0f;
			colShape = new btSphereShape(btScalar(radius));
		}
		else if (shape_type == "Box") {
			rhea::BoxColliderShape* boxShape = static_cast<rhea::BoxColliderShape*>(shape);
			float hx = boxShape->half_extents.x;
			float hy = boxShape->half_extents.y;
			float hz = boxShape->half_extents.z;

			if (hx < 0.0005f) hx = 1.0f;
			if (hy < 0.0005f) hy = 1.0f;
			if (hz < 0.0005f) hz = 1.0f;

			colShape = new btBoxShape(btVector3(hx, hy, hz));
		}
		else {
			// assume mesh collider, for static object we use the triangles, for dynamic objects we handle it not correctly...

			if (!isDynamic() && !m_isKinematic) {
				TriangleSoup tri;
				rhea::Application* app = m_item->getApplication();
				if (!ExtractTrianglesFromScene(app, m_item, tri) || tri.indices.empty()) {
					LOG(LOG_ERROR) << "BtBody: no collision triangles for item " << m_item->getPath();
					colShape = new btBoxShape(btVector3(1, 1, 1));
					return false;
				}
				else {
					btTriangleMesh* triMesh = new btTriangleMesh();
					for (size_t i = 0; i + 2 < tri.indices.size(); i += 3) {
						auto& a = tri.positions[tri.indices[i]];
						auto& b = tri.positions[tri.indices[i + 1]];
						auto& c = tri.positions[tri.indices[i + 2]];
						triMesh->addTriangle(
							btVector3(a.x, a.y, a.z),
							btVector3(b.x, b.y, b.z),
							btVector3(c.x, c.y, c.z)
						);
					}
					colShape = new btBvhTriangleMeshShape(triMesh, true);
				}
				/*
				// for static object we use the triangles
				Mesh* mesh = m_item->getMesh();
				if (!mesh) {
					return false;
				}
				VertexBuffer* vb = mesh->getVertexBuffer();
				if (vb && vb->size()) {
					for (size_t idx = 0; idx < mesh->getNumSubmeshes(); idx++) {
						std::vector<unsigned long> indices = mesh->getIndices(idx);
						btTriangleMesh* triMesh = new btTriangleMesh();
						for (size_t i = 0; i < indices.size(); i += 3) {
							size_t j = indices[i];
							math::Vector3 vertex = (math::Vector3)(*vb)[j];
							btVector3 v1 = btVector3(vertex.x, vertex.y, vertex.z);
							j = indices[i + 1];
							vertex = (math::Vector3)(*vb)[j];
							btVector3 v2 = btVector3(vertex.x, vertex.y, vertex.z);
							j = indices[i + 2];
							vertex = (math::Vector3)(*vb)[j];
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
				*/
			}
			else {
				// for dynamic objects we handle it not correctly...
				colShape = new btBoxShape(btVector3(1, 1, 1));
			}
		}

		//TODO (for cleanup and reuse)		collisionShapes.push_back(colShape);
		return colShape;
	}
	
	BulletPhysicsBody::~BulletPhysicsBody()
	{
		if (m_body) {
			auto motionState = m_body->getMotionState();
			if (motionState) delete motionState;
			if (m_world) m_world->removeRigidBody(m_body);
			delete m_body;
			m_body = nullptr;
		}
	}

	void BulletPhysicsBody::move(math::Vector3 move)
	{
		if (move == math::Vector3::ZERO &&
			m_movement != math::Vector3::ZERO) {
			stop();
		}
		m_movement = move;
	}

	void BulletPhysicsBody::turn(float turn)
	{
		m_turn = turn;
	}

	void BulletPhysicsBody::stop()
	{
		m_body->setLinearVelocity(btVector3(0.0, 0.0, 0.0));
		m_body->setActivationState(ACTIVE_TAG);
	}

	void BulletPhysicsBody::setPosition(const math::Vector3& position)
	{
		btTransform transform;
		transform.setIdentity();

		transform.setOrigin(btVector3(
			btScalar(position.x),
			btScalar(position.y),
			btScalar(position.z)));

		m_body->getMotionState()->setWorldTransform(transform);
	}

	void BulletPhysicsBody::setOrientation(const math::Quaternion& rot)
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

	void BulletPhysicsBody::setAngularVelocity(const math::Vector3& velocity)
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

	void BulletPhysicsBody::applyOrientation(const math::Quaternion& rot)
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

	// helper functions for converting math::Vector3 and math::Quaternion to Bullet types
	static inline btVector3 toBt(const math::Vector3& v)
	{
		return btVector3(v.x, v.y, v.z);
	}

	static inline btQuaternion toBt(const math::Quaternion& q)
	{
		return btQuaternion(q.x, q.y, q.z, q.w);
	}
}

