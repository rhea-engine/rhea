/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_PHYSICS_BULLET_BODY_H
#define RHEA_PHYSICS_BULLET_BODY_H

#include <Rhea/Core/Position.h>

#include <Rhea/Physics/PhysicsBody.h>
#include <Rhea/Physics/PhysicsBodyDescriptor.h>

class btRigidBody;
class btDynamicsWorld;
class btCompoundShape;
class btCollisionShape;
class btStridingMeshInterface;

namespace rhea
{
	class Item;
	class PhysicsDataComponent;
	class ColliderComponent;
	class ColliderShape;
}

namespace rhea::physics::bullet
{

	class BulletPhysicsBody : public rhea::PhysicsBody
	{
	public:
		BulletPhysicsBody(btDynamicsWorld* world, rhea::Item* item);
		~BulletPhysicsBody();

		virtual bool init();
		// create collision shape from physics data component
		// the shape is owned by the BulletPhysicsBody and will be deleted in the destructor
		btCollisionShape* createCollisionShape(PhysicsDataComponent* pdc);
		btCollisionShape* createColliderShape(ColliderShape* shape);

		// move by vector
		void move(math::Vector3 move) override;

		// turn by radian angle
		void turn(float turn) override;

		// stop the current movement
		void stop() override;

		void setPosition(const math::Vector3& position) override;
		void setOrientation(const math::Quaternion& rot) override;

		// functions for setting attributes
		void setAngularVelocity(const math::Vector3& velocity) override;

		void DisableMassUpdate() override;
		void EnableMassUpdate() override;

		void applyImpulse(const math::Vector3& impulse) override;
		void applyOrientation(const math::Quaternion& rot) override;

		void activate();

		void setFriction(float friction);
		
		btRigidBody* getBody() { return m_body; }		
		
	protected:
		math::Vector3 m_movement;
		float m_turn;
		btRigidBody* m_body;		
		float m_upwards;
		btDynamicsWorld* m_world;
		float m_mass;
		std::vector<btCollisionShape*> m_shapes;
		std::vector<btStridingMeshInterface*> m_mesh_interfaces;
	};
}

#endif // !RHEA_PHYSICS_BULLET_BODY_H