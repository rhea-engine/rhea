/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "BtCharacter.h"
#include "BtGhost.h"

#include <Rhea/Core/Item.h>
#include <Rhea/Components/PhysicsDataComponent.h>
#include <Rhea/Components/ColliderComponent.h>
#include <Psio/Log.h>

#include <bullet/src/btBulletDynamicsCommon.h>
#include <bullet/src/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <bullet/src/BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

using math::Vector3;

namespace rhea::physics::bullet
{
	// class from btKinematicCharacterController
	class btKinematicClosestNotMeRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
	{
	public:
		btKinematicClosestNotMeRayResultCallback(btCollisionObject* me, btVector3 from, btVector3 to) : 
			btCollisionWorld::ClosestRayResultCallback(from, to)
		{
			m_me = me;
		}

		virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
		{
			if (rayResult.m_collisionObject == m_me)
				return 1.0;

			return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
		}

	protected:
		btCollisionObject* m_me;
	};

	// class from btKinematicCharacterController
	class btKinematicClosestNotMeConvexResultCallback : public btCollisionWorld::ClosestConvexResultCallback
	{
	public:
		btKinematicClosestNotMeConvexResultCallback(btCollisionObject* me, const btVector3& up, btScalar minSlopeDot)
			: btCollisionWorld::ClosestConvexResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0)), m_me(me), m_up(up), m_minSlopeDot(minSlopeDot)
		{
		}

		virtual btScalar addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
		{
			if (convexResult.m_hitCollisionObject == m_me)
				return btScalar(1.0);

			if (!convexResult.m_hitCollisionObject->hasContactResponse())
				return btScalar(1.0);

			btVector3 hitNormalWorld;
			if (normalInWorldSpace)
			{
				hitNormalWorld = convexResult.m_hitNormalLocal;
			}
			else
			{
				///need to transform normal into worldspace
				hitNormalWorld = convexResult.m_hitCollisionObject->getWorldTransform().getBasis() * convexResult.m_hitNormalLocal;
			}

			hitNormalWorld.normalize();

			if (hitNormalWorld.dot(m_up) > m_minSlopeDot)
			{
				///ignore convex result if it is a slope and the character cannot walk on the slope
				return btScalar(1.0);
			}
			
			return ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
		}

	protected:
		btCollisionObject* m_me;
		const btVector3 m_up;
		btScalar m_minSlopeDot;
	};

	class ClosestNotMeConvexResultCallback
		: public btCollisionWorld::ClosestConvexResultCallback
	{
	public:
		explicit ClosestNotMeConvexResultCallback(
			btCollisionObject* me,
			const btVector3& from,
			const btVector3& to
		)
			: btCollisionWorld::ClosestConvexResultCallback(from, to),
			m_me(me)
		{}

		btScalar addSingleResult(
			btCollisionWorld::LocalConvexResult& result,
			bool normalInWorldSpace
		) override
		{
			if (result.m_hitCollisionObject == m_me)
				return btScalar(1.0);

			if (!result.m_hitCollisionObject->hasContactResponse())
				return btScalar(1.0);

			return ClosestConvexResultCallback::addSingleResult(
				result,
				normalInWorldSpace
			);
		}

	private:
		btCollisionObject* m_me;
	};

	BulletPhysicsCharacter::BulletPhysicsCharacter(btDynamicsWorld* world, rhea::Item* item) :
		m_world(world),
		m_move_state(moveState::GROUND),
		m_last_position(0.0f,0.0f,0.0f),
		m_foot_contact(false),
		m_head_contact(false),
		m_foot_contact_pos(0.0f, 0.0f,0.0f),
		m_foot_contact_normal(0.0f,0.0f,0.0f),
		m_ghost_object(nullptr),
		m_hitting_wall(false),
		m_position(nullptr),
		BulletPhysicsBody(world, item)
	{
		// prepare default values
		m_step_height = item->getValue("step_height", 0.1f);

		auto* pdc = item->getComponent<rhea::PhysicsDataComponent>();
		if (!pdc) return;

		if (pdc->colliders.size() == 0 || pdc->colliders[0]->shape==nullptr || pdc->colliders[0]->shape->getShapeType() != "Capsule") {
			// log error
			LOG(LOG_ERROR) << "BulletPhysicsCharacter requires a capsule shape for collision, but got " << (pdc->colliders.size() > 0 ? (pdc->colliders[0]->shape ? pdc->colliders[0]->shape->getShapeType() : "None") : "None");
			return;
		}

		rhea::CapsuleColliderShape* shape = static_cast<rhea::CapsuleColliderShape*>(pdc->colliders[0]->shape);

		// total height of the capsule
		m_height = shape->height;
		m_radius = shape->radius;

		// make sure we have sensible values

	/*	if (m_height - m_step_height <= m_radius * 2.0f) {
			float length = 0.1f;
			m_radius = (m_height * 0.5f) - (m_step_height * 0.5f);
			item->setValue("collision_shape_radius", m_radius);
			m_levitation = m_step_height + m_radius;
		}
		else {
		*/
			float length = m_height - m_step_height - m_radius * 2.0f;
			m_levitation = m_height - m_radius - length / 2.0f;
		//}		
		m_gravity = pdc->rigid_body.gravity;

		float cylinder_height = m_height - 2.0f * m_radius;

		m_ghost_offset_y = m_step_height + m_height * 0.5f;
		m_capsule_offset = m_ghost_offset_y + m_height * 0.5f;

		// minimal y postion check
		m_check_ground = pdc->rigid_body.ground_check;
		m_minimal_ground = pdc->rigid_body.ground_height;

		// create the ghost object
		m_ghost = new BulletPhysicsGhost(world, item, m_radius, m_height, m_ghost_offset_y);
	}

	bool BulletPhysicsCharacter::init()
	{
		// init ghost object
		if (m_ghost && m_ghost->init()) {
			m_ghost_object = m_ghost->getObject();
			return true;
		}
		return false;
	}

	BulletPhysicsCharacter::~BulletPhysicsCharacter()
	{
		if (m_ghost)
			delete m_ghost;
	}	

	void BulletPhysicsCharacter::stop()
	{
		// dummy - overwrite base class
	}

	bool BulletPhysicsCharacter::isOnGround() const
	{
		return (m_move_state == moveState::GROUND);
	}
	
	bool BulletPhysicsCharacter::isMoveControllable() const
	{
		return m_move_state == moveState::GROUND || m_move_state == moveState::FLY;
	}

	void BulletPhysicsCharacter::setPosition(const math::Vector3& position)
	{
		if (m_ghost)
			m_ghost->setPosition(position);
	}

	bool BulletPhysicsCharacter::jump(math::Vector3& position)
	{
		if (m_move_state == moveState::GROUND) {
			// check if jumping possible
			m_currentPosition = btVector3(position.x, position.y, position.z);
			m_last_position = m_currentPosition;
			updateHeadContact();
			if (!m_head_contact) {
				m_move_state = moveState::JUMP;
				position.y += m_step_height + 0.01f;
				return true;
			}
		}
		return false;
	}

	void BulletPhysicsCharacter::fly()
	{
		m_move_state = moveState::FLY;
	}

	void BulletPhysicsCharacter::setOrientation(const math::Quaternion& rot)
	{
		if (m_ghost)
			m_ghost->setOrientation(rot);
	}

	bool BulletPhysicsCharacter::sweepGravity()
	{
		btVector3 gravityMove = m_gravity_v;

		if (gravityMove.length2() < SIMD_EPSILON)
			return false;

		auto* shape =
			static_cast<btConvexShape*>(
				m_ghost_object->getCollisionShape()
				);

		if (!shape) {
			m_currentPosition += gravityMove;
			return false;
		}

		btVector3 ghostOffset(0, m_ghost_offset_y, 0);

		btTransform from;
		from.setIdentity();
		from.setOrigin(m_currentPosition + ghostOffset);

		btTransform to;
		to.setIdentity();
		to.setOrigin(m_currentPosition + gravityMove + ghostOffset);

		ClosestNotMeConvexResultCallback callback(
			m_ghost_object,
			from.getOrigin(),
			to.getOrigin()
		);

		callback.m_collisionFilterGroup =
			m_ghost_object->getBroadphaseHandle()->m_collisionFilterGroup;

		callback.m_collisionFilterMask =
			m_ghost_object->getBroadphaseHandle()->m_collisionFilterMask;

		m_world->convexSweepTest(shape, from, to, callback);

		if (!callback.hasHit()) {
			m_currentPosition += gravityMove;
			return false;
		}

		btScalar safeFraction = btMax(
			btScalar(0.0),
			callback.m_closestHitFraction - btScalar(0.001)
		);

		m_currentPosition += gravityMove * safeFraction;

		btVector3 normal =
			callback.m_hitNormalWorld.normalized();

		btScalar dotUp = normal.dot(m_up);

		if (gravityMove.getY() <= 0.0f &&
			dotUp >= m_min_slope_dot)
		{
			// hitPointWorld lies on the surface
			m_currentPosition.setY(
				callback.m_hitPointWorld.getY()
			);

			m_foot_contact_pos = callback.m_hitPointWorld;
			m_foot_contact_normal = normal;
			m_foot_contact = true;
			m_move_state = moveState::GROUND;
			return true;
		}

		// steep slope
		// project remaining motion
		btVector3 remainingMove = gravityMove * (btScalar(1.0) - safeFraction);

		btVector3 slideMove = remainingMove - normal * remainingMove.dot(normal);

		if (slideMove.length2() < SIMD_EPSILON) {
			m_foot_contact = false;
			return false;
		}

		// a small offset to avoid getting stuck in the surface (0.002 is a magic number from btKinematicCharacterController)
		m_currentPosition += normal * btScalar(0.002);

		btTransform slideFrom;
		slideFrom.setIdentity();
		slideFrom.setOrigin(
			m_currentPosition + ghostOffset
		);

		btTransform slideTo;
		slideTo.setIdentity();
		slideTo.setOrigin(
			m_currentPosition + slideMove + ghostOffset
		);

		ClosestNotMeConvexResultCallback slideCallback(
			m_ghost_object,
			slideFrom.getOrigin(),
			slideTo.getOrigin()
		);

		slideCallback.m_collisionFilterGroup =
			m_ghost_object->getBroadphaseHandle()->m_collisionFilterGroup;

		slideCallback.m_collisionFilterMask =
			m_ghost_object->getBroadphaseHandle()->m_collisionFilterMask;

		m_world->convexSweepTest(
			shape,
			slideFrom,
			slideTo,
			slideCallback
		);

		if (!slideCallback.hasHit()) {
			m_currentPosition += slideMove;
		}
		else {
			btScalar slideFraction = btMax(
				btScalar(0.0),
				slideCallback.m_closestHitFraction
				- btScalar(0.001)
			);

			m_currentPosition += slideMove * slideFraction;
		}

		m_move_state = moveState::FALL;
		m_foot_contact = false;
		return false;
	}

	void BulletPhysicsCharacter::detectGhostContacts()
	{
		btManifoldArray manifoldArray;
		btBroadphasePairArray& pairArray = m_ghost_object->getOverlappingPairCache()->getOverlappingPairArray();
		int numPairs = pairArray.size();

		// reset collision checks
		m_hitting_wall = false;
		m_surface_hits_normals.clear();

		for (int i = 0; i < numPairs; ++i) {
			manifoldArray.clear();

			const btBroadphasePair& pair = pairArray[i];

			btBroadphasePair* collisionPair = m_world->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);

			if (!collisionPair) continue;

			if (collisionPair->m_algorithm)
				collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);

			for (int j = 0; j < manifoldArray.size(); j++) {
				btPersistentManifold* manifold = manifoldArray[j];

				bool isFirstBody = manifold->getBody0() == m_ghost_object;

				btScalar direction = isFirstBody ? btScalar(-1.0) : btScalar(1.0);

				for (int p = 0; p < manifold->getNumContacts(); ++p) {
					const btManifoldPoint& pt = manifold->getContactPoint(p);

					if (pt.getDistance() < 0.f) {
						const btVector3& ptA = pt.getPositionWorldOnA();
						const btVector3& ptB = pt.getPositionWorldOnB();
						const btVector3& normalOnB = pt.m_normalWorldOnB;
						const btVector3 pNormal2 = ptB - ptA;
						const btVector3 pNormal = ptA - ptB;

						// store collisions here

						/*printf("Collision! p(%d) dir=%f nOnB(%f, %f, %f), normal=(%f, %f, %f)\n", p, direction,
							normalOnB.getX(), normalOnB.getY(), normalOnB.getZ(),
							pNormal.getX(), pNormal.getY(), pNormal.getZ()
							);
							*/
						m_hitting_wall = true;
//						m_surface_hits_normals.push_back(Vector3(normalOnB.getX(), normalOnB.getY(), normalOnB.getZ()));
						m_surface_hits_normals.push_back(Vector3(pNormal.getX(), pNormal.getY(), pNormal.getZ()));
					}
				}
			}
		}
	}



	void BulletPhysicsCharacter::moveStep(rhea::Position* pos, math::Vector3& move)
	{
		Vector3 trans = pos->getRotation() * move;

		btVector3 fromPos = m_currentPosition;
		btVector3 desiredMove(trans.x, trans.y, trans.z);
		btVector3 toPos = fromPos + desiredMove;

		if (desiredMove.length2() < SIMD_EPSILON) {
			return;
		}

		btVector3 ghostOffset(0, m_ghost_offset_y, 0);

		btTransform from;
		from.setIdentity();
		from.setOrigin(fromPos + ghostOffset);

		btTransform to;
		to.setIdentity();
		to.setOrigin(toPos + ghostOffset);

		auto* shape = static_cast<btConvexShape*>(m_ghost_object->getCollisionShape());
		if (!shape) {
			m_currentPosition = toPos;
			return;
		}
		
		btKinematicClosestNotMeConvexResultCallback callback(m_ghost_object, btVector3(0.0f, 1.0f, 0.0f), m_min_slope_dot);
		callback.m_collisionFilterGroup = m_ghost_object->getBroadphaseHandle()->m_collisionFilterGroup;
		callback.m_collisionFilterMask = m_ghost_object->getBroadphaseHandle()->m_collisionFilterMask;

		m_world->convexSweepTest(shape, from, to, callback);

		if (callback.hasHit()) {
			m_hitting_wall = true;

			// we hit something, move only up to the hit point
			float safeFraction = callback.m_closestHitFraction * 0.9f;
			m_currentPosition = fromPos.lerp(toPos, safeFraction);
			return;

			// slide along the wall
			// not working yet, needs more work
			btVector3 hitNormal = callback.m_hitNormalWorld.normalized();
			//btScalar safeFraction = btMax(btScalar(0.0), callback.m_closestHitFraction - m_added_margin);

			btVector3 allowedMove = desiredMove * safeFraction;
			btVector3 firstPos = fromPos + allowedMove;

			btVector3 remainingMove = desiredMove - allowedMove;

			// Project remaining movement onto the wall plane.
			btVector3 slideMove = remainingMove - hitNormal * remainingMove.dot(hitNormal);

			if (slideMove.length2() < SIMD_EPSILON/* || callback.m_closestHitFraction < btScalar(0.001)*/) {
				m_currentPosition = firstPos;
				return;
			}

			// Limit slide amount at first while debugging.
			//slideMove *= btScalar(0.5);

			// Sweep again for the slide.
			btTransform slideFrom;
			slideFrom.setIdentity();
			slideFrom.setOrigin(firstPos + btVector3(0, m_ghost_offset_y, 0));

			btTransform slideTo;
			slideTo.setIdentity();
			slideTo.setOrigin(firstPos + slideMove + btVector3(0, m_ghost_offset_y, 0));

			btKinematicClosestNotMeConvexResultCallback slideCallback(
				m_ghost_object,
				m_up,
				m_min_slope_dot
			);

			slideCallback.m_closestHitFraction = 1.0f;

			m_world->convexSweepTest(shape, slideFrom, slideTo, slideCallback);

			if (slideCallback.hasHit()) {
				btScalar slideSafeFraction =
					btMax(btScalar(0.0), slideCallback.m_closestHitFraction - btScalar(0.02));

				m_currentPosition = firstPos.lerp(firstPos + slideMove, slideSafeFraction);
			}
			else {
				m_currentPosition = firstPos + slideMove;
			}

			return;
		}

		m_hitting_wall = false;
		m_currentPosition = toPos;
	}

	bool BulletPhysicsCharacter::updateFootContact(bool ccd)
	{
		//		Vector3 pFrom = m_position->getTranslation();
		btVector3 step_height = btVector3(0, m_step_height, 0);
		btVector3 from = m_currentPosition + step_height;
		btVector3 to = m_currentPosition - step_height;

		btKinematicClosestNotMeRayResultCallback results(m_ghost_object, from, to);
		results.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

		m_world->rayTest(from, to, results);

		// no hit: we are in the air!
		if (!results.hasHit()) {
			if (ccd && (m_last_position!=m_currentPosition)) {
				// continous collision detection
				// check for hit on last move
				btKinematicClosestNotMeRayResultCallback results_back(m_ghost_object, m_currentPosition, m_last_position);
				//results_back.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;
				m_world->rayTest(m_currentPosition, m_last_position, results_back);
				if (results_back.hasHit()) {
					// we passed something
					m_foot_contact_pos = results_back.m_hitPointWorld;
					m_foot_contact_normal = results_back.m_hitNormalWorld;
					m_foot_contact = true;
					m_currentPosition.setY(m_foot_contact_pos.getY());
					//printf("touchdown\n");
					return true;
				}
			}
			m_foot_contact = false;
			return false;
		}

		// steep slope check: if the normal is too steep we are not on ground
		btScalar dotUp = results.m_hitNormalWorld.normalized().dot(btVector3(0, 1, 0));
		float groundY = results.m_hitPointWorld.getY();

		if (dotUp < 0.707f) {
			if (m_currentPosition.getY() < groundY) {
				m_currentPosition.setY(groundY);
			}

			m_foot_contact = false;
			return false;
		}

		// we are near the ground
		// m_closestHitFraction < 0.5 means we have to climb
		// > 0.5 means we have to descend

	/*	m_move_state = moveState::GROUND;

		if (btFabs(results.m_closestHitFraction - 0.5f) < SIMD_EPSILON) {
			// exactly on ground, prevent jittering
			m_foot_contact_pos = results.m_hitPointWorld;
			m_foot_contact_normal = results.m_hitNormalWorld;
			m_foot_contact = true;
			return true;
		}*/

#if 0
		if (results.m_closestHitFraction < 0.5f)
			printf("foot_contact\n");
		else if (results.m_closestHitFraction < 0.5f)
			printf("slope\n");
#endif
		m_foot_contact_pos = results.m_hitPointWorld;
		m_foot_contact_normal = results.m_hitNormalWorld;
		m_foot_contact = true;
		// update current position to be on the ground
		// but prevent jittering by only updating if we are more than 2mm away from the ground
	//	float groundY = results.m_hitPointWorld.getY();
		float correction = groundY - m_currentPosition.getY();
		if (btFabs(correction) > 0.002f)
			m_currentPosition.setY(m_currentPosition.getY() + correction);
		else
		m_currentPosition.setY(m_foot_contact_pos.getY());
		m_move_state = moveState::GROUND;
		return true;
	}

	void BulletPhysicsCharacter::updateHeadContact()
	{
		btVector3 from = m_currentPosition + btVector3(0.0f, m_capsule_offset, 0.0f);
		btVector3 to = btVector3(from + btVector3(0.0f, m_height * 20.0f, 0.0f));

		btKinematicClosestNotMeRayResultCallback results(m_ghost_object, from, to);
		results.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

		m_world->rayTest(from, to, results);

		if (!results.hasHit()) {
			m_head_contact = false;
			return;
		}

		//printf("head_contact\n");
		m_head_contact = true;
		m_head_contact_pos = results.m_hitPointWorld;
	}

	math::Vector3 BulletPhysicsCharacter::update(rhea::Position* pos, math::Vector3& move, math::Vector3& gravity)
	{
		m_position = pos;
		Vector3 position = pos->getTranslation();
		m_currentPosition = btVector3(position.x, position.y, position.z);
		m_gravity_v = btVector3(gravity.x, gravity.y, gravity.z);

		Vector3 dir = pos->getRotation() * move;

		updateHeadContact();
	
		detectGhostContacts();

		switch (m_move_state) {
		case moveState::GROUND:
		case moveState::FLY:
			moveStep(pos, move);
			// check if we are on ground
			if (!updateFootContact())
				m_move_state = moveState::FALL;
			break;
		case moveState::JUMP:
			// move by given vector (which includes y)
			moveStep(pos, move);
			if (sweepGravity()) {
				m_move_state = moveState::GROUND;
			}
			else {
				updateFootContact(true);
			}
			break;
		case moveState::FALL:
			if (sweepGravity()) {
				m_move_state = moveState::GROUND;
			}
			else {
				updateFootContact(true);
			}
			break;
		}

		// safety check - note: if enabled this means we cant go below ground!
		if (m_check_ground)
			if (m_currentPosition.getY() < m_minimal_ground - 0.01f) {
				m_currentPosition.setY(m_minimal_ground);
				m_move_state = moveState::GROUND;
			}
		Vector3 target_position = Vector3(m_currentPosition.getX(), m_currentPosition.getY(), m_currentPosition.getZ());
		
		m_last_position = m_currentPosition;

		return target_position;
	}
}