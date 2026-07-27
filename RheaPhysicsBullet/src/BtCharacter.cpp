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

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>

#if DEBUG_CHARACTER_EVENTS || DEBUG_CHARACTER_VERBOSE
#define CHAR_LOG(severity) LOG_COND(Log::logger().level()>severity, severity) & LogMessage(severity).stream()
#else
#define CHAR_LOG(severity) if(false) LogMessage(severity).stream()
#endif

using math::Vector3;

namespace rhea::physics::bullet
{
	// class from btKinematicCharacterController
	class characterRayCallback : public btCollisionWorld::ClosestRayResultCallback
	{
	public:
		characterRayCallback(btCollisionObject* me, btVector3 from, btVector3 to) : 
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
	class characterSweepCallback : public btCollisionWorld::ClosestConvexResultCallback
	{
	public:
		characterSweepCallback(btCollisionObject* me, const btVector3& up, btScalar minSlopeDot)
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
				/// Horizontal movement must not be blocked by walkable ground.
				return btScalar(1.0);
			}
			
			return ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
		}

	protected:
		btCollisionObject* m_me;
		const btVector3 m_up;
		btScalar m_minSlopeDot;
	};

	class WalkableGroundSweepCallback
		: public btCollisionWorld::ClosestConvexResultCallback
	{
	public:
		explicit WalkableGroundSweepCallback(
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
		m_foot_contact(false),
		m_head_contact(false),
		m_foot_contact_normal(0.0f,0.0f,0.0f),
		m_ghost_object(nullptr),
		BulletPhysicsBody(world, item)
	{
		// prepare default values
		m_step_height = item->getValue("step_height", 0.1f);
		float max_angle = item->getValue("max_slope_angle", 45.0f);
		m_min_slope_dot = btCos(btRadians(max_angle));

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

		// capsule middle lies a half capsule height above the feet.
		m_ghost_offset_y = m_height * 0.5f;

		// head position relative to the foot position.
		m_capsule_offset = m_height;

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
			updateHeadContact();
			if (!m_head_contact) {
				m_foot_contact = false;
				m_move_state = moveState::JUMP;
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

#if DEBUG_CHARACTER_VERBOSE
		CHAR_LOG(LOG_DEBUG)
			<< "[GRAVITY]"
			<< " tick=" << m_debug_tick
			<< " posY=" << m_currentPosition.y()
			<< " moveY=" << gravityMove.y();
#endif
		if (gravityMove.length2() < SIMD_EPSILON) {
			// No gravity to apply - this can cause the character to get stuck in FALL state
			return false;
		}

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

		WalkableGroundSweepCallback callback(
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
			m_foot_contact = false;
			return false;
		}
#if DEBUG_CHARACTER_EVENTS || DEBUG_CHARACTER_VERBOSE
		const btVector3 hitNormal =
			callback.m_hitNormalWorld.normalized();

		const btVector3 hitPoint =
			callback.m_hitPointWorld;

		const btScalar groundDot =
			hitNormal.dot(m_up);

		const btScalar hitHeight =
			hitPoint.y() - from.getOrigin().y();

		CHAR_LOG(LOG_DEBUG)
			<< "[MOVE-HIT]"
			<< " tick=" << m_debug_tick
			<< " fraction=" << callback.m_closestHitFraction
			<< " normal=("
			<< hitNormal.x() << ","
			<< hitNormal.y() << ","
			<< hitNormal.z() << ")"
			<< " dotUp=" << groundDot
			<< " point=("
			<< hitPoint.x() << ","
			<< hitPoint.y() << ","
			<< hitPoint.z() << ")"
			<< " relativeHeight=" << hitHeight
			<< " object="
			<< callback.m_hitCollisionObject;
#endif

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

		// a small offset to avoid getting stuck in the surface
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

		WalkableGroundSweepCallback slideCallback(
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

	void BulletPhysicsCharacter::moveStep(rhea::Position* pos, math::Vector3& move)
	{
		Vector3 trans = pos->getRotation() * move;

		btVector3 fromPos = m_currentPosition;
		btVector3 desiredMove(trans.x, trans.y, trans.z);
#if DEBUG_CHARACTER_VERBOSE
		const btVector3 horizontalStart = m_currentPosition;

		CHAR_LOG(LOG_DEBUG)
			<< "[MOVE]"
			<< " tick=" << m_debug_tick
			<< " state=" << static_cast<int>(m_move_state)
			<< " from=("
			<< fromPos.x() << ","
			<< fromPos.y() << ","
			<< fromPos.z() << ")"
			<< " requested=("
			<< desiredMove.x() << ","
			<< desiredMove.y() << ","
			<< desiredMove.z() << ")"
			<< " requestedLength=" << desiredMove.length();
#endif
		// adjust for slopes
		if (m_move_state == moveState::GROUND &&
			m_foot_contact_normal.length2() > SIMD_EPSILON)
		{
			const btVector3 groundNormal =
				m_foot_contact_normal.normalized();

			const btScalar groundDot =
				groundNormal.dot(m_up);

			if (groundDot >= m_min_slope_dot) {
				const btScalar originalLength =
					desiredMove.length();

				// project movement onto the slope
				desiredMove -=
					groundNormal * desiredMove.dot(groundNormal);

				// maintain constant speed along the slope
				if (desiredMove.length2() > SIMD_EPSILON) {
					desiredMove.normalize();
					desiredMove *= originalLength;
				}
			}
		}


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
		
		characterSweepCallback callback(m_ghost_object, btVector3(0.0f, 1.0f, 0.0f), m_min_slope_dot);
		callback.m_collisionFilterGroup = m_ghost_object->getBroadphaseHandle()->m_collisionFilterGroup;
		callback.m_collisionFilterMask = m_ghost_object->getBroadphaseHandle()->m_collisionFilterMask;

		m_world->convexSweepTest(shape, from, to, callback);

		if (callback.hasHit()) {
			const btVector3 hitNormal =
				callback.m_hitNormalWorld.normalized();

			const btScalar groundDot =
				hitNormal.dot(m_up);
#if DEBUG_CHARACTER_EVENTS || DEBUG_CHARACTER_VERBOSE
			const btVector3 hitPoint =
				callback.m_hitPointWorld;

			const btScalar hitHeight =
				hitPoint.y() - fromPos.y();

			CHAR_LOG(LOG_DEBUG)
				<< "[MOVE-HIT]"
				<< " tick=" << m_debug_tick
				<< " fraction=" << callback.m_closestHitFraction
				<< " normal=("
				<< hitNormal.x() << ","
				<< hitNormal.y() << ","
				<< hitNormal.z() << ")"
				<< " dotUp=" << groundDot
				<< " point=("
				<< hitPoint.x() << ","
				<< hitPoint.y() << ","
				<< hitPoint.z() << ")"
				<< " relativeHeight=" << hitHeight
				<< " object="
				<< callback.m_hitCollisionObject;
#endif
			// Walkable surface, not a wall.
			if (groundDot >= m_min_slope_dot) {
				m_currentPosition = toPos;
				return;
			}
			else {
#if DEBUG_CHARACTER_VERBOSE
				CHAR_LOG(LOG_DEBUG)
					<< "[MOVE-BLOCKED]"
					<< " tick=" << m_debug_tick
					<< " newY="
					<< (m_currentPosition.y() + m_gravity_v.y());
#endif
			}


			// Check if the hit can be overcome by raising by step_height.
			const btScalar stepSkin = btScalar(0.02f);
			const btVector3 stepOffset =
				m_up * (m_step_height + stepSkin);

			btTransform raisedFrom = from;
			btTransform raisedTo = to;

			raisedFrom.setOrigin(from.getOrigin() + stepOffset);
			raisedTo.setOrigin(to.getOrigin() + stepOffset);

			WalkableGroundSweepCallback stepCallback(
				m_ghost_object,
				raisedFrom.getOrigin(),
				raisedTo.getOrigin()
			);

			stepCallback.m_collisionFilterGroup =
				m_ghost_object->getBroadphaseHandle()
				->m_collisionFilterGroup;

			stepCallback.m_collisionFilterMask =
				m_ghost_object->getBroadphaseHandle()
				->m_collisionFilterMask;

			m_world->convexSweepTest(
				shape,
				raisedFrom,
				raisedTo,
				stepCallback
			);
#if DEBUG_CHARACTER_VERBOSE

			CHAR_LOG(LOG_DEBUG)
				<< "[STEP-SWEEP]"
				<< " tick=" << m_debug_tick
				<< " hit=" << stepCallback.hasHit()
				<< " fraction="
				<< stepCallback.m_closestHitFraction;
#endif
			if (!stepCallback.hasHit()) {
				// Continue moving horizontally.
				//
				// If it's a step, updateFootContact() will adjust the height afterwards.
				//
				// If it's an edge, the character will switch to FALL.
				m_currentPosition = toPos;
				return;
			}

			trySlide(shape, fromPos, desiredMove, callback.m_closestHitFraction, hitNormal);

			return;
		}

		m_currentPosition = toPos;
#if DEBUG_CHARACTER_VERBOSE
		const btVector3 actualMove =
			m_currentPosition - horizontalStart;

		CHAR_LOG(LOG_DEBUG)
			<< "[MOVE-RESULT]"
			<< " tick=" << m_debug_tick
			<< " actual=("
			<< actualMove.x() << ","
			<< actualMove.y() << ","
			<< actualMove.z() << ")"
			<< " actualLength=" << actualMove.length();
#endif
	}

	bool BulletPhysicsCharacter::updateFootContact()
	{
		const btScalar skin = 0.02f;

		const btScalar probeUp =
			m_step_height + skin;

		const btScalar probeDown =
			m_step_height * 1.25f + skin;

		const btVector3 from =
			m_currentPosition + m_up * probeUp;

		const btVector3 to =
			m_currentPosition - m_up * probeDown;

		characterRayCallback results(m_ghost_object, from, to);
		results.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

		m_world->rayTest(from, to, results);

#if DEBUG_CHARACTER_VERBOSE
		CHAR_LOG(LOG_DEBUG)
			<< "[GROUND-RAY]"
			<< " tick=" << m_debug_tick
			<< " fromY=" << from.y()
			<< " toY=" << to.y()
			<< " hit=" << results.hasHit();
#endif
		// no hit: we are in the air!
		if (!results.hasHit()) {
			CHAR_LOG(LOG_DEBUG) << "[FootContact] No ground hit!";
			m_foot_contact = false;
			return false;
		}
		else {
#if DEBUG_CHARACTER_EVENTS || DEBUG_CHARACTER_VERBOSE
			CHAR_LOG(LOG_DEBUG)
				<< "[GROUND-HIT]"
				<< " tick=" << m_debug_tick
				<< " fraction=" << results.m_closestHitFraction
				<< " groundY=" << results.m_hitPointWorld.y()
				<< " playerY=" << m_currentPosition.y()
				<< " distance="
				<< (m_currentPosition.y()
					- results.m_hitPointWorld.y())
				<< " dotUp="
				<< results.m_hitNormalWorld
				.normalized()
				.dot(m_up);
#endif
		}

		// steep slope check: if the normal is too steep we are not on ground
		btScalar dotUp = results.m_hitNormalWorld.normalized().dot(btVector3(0, 1, 0));
		float groundY = results.m_hitPointWorld.getY();

		CHAR_LOG(LOG_DEBUG) << "[FootContact] Hit! CharY=" << m_currentPosition.getY() << ", GroundY=" << groundY << ", Dist=" << (m_currentPosition.getY() - groundY) << ", DotUp=" << dotUp;

		if (dotUp < m_min_slope_dot) {
			CHAR_LOG(LOG_DEBUG) << "[FootContact] -> SLOPE TOO STEEP (dotUp=" << dotUp << ")";
			m_foot_contact = false;
			return false;
		}

		// CRITICAL: Check if ground is close enough to be considered "on ground"
		float distanceToGround = m_currentPosition.getY() - groundY;
		

		// If character is BELOW ground (sunken in), allow correction up to step_height
		// This handles cases where physics pushed character slightly into ground
		if (distanceToGround < 0.0f && btFabs(distanceToGround) > m_step_height) {
			CHAR_LOG(LOG_DEBUG) << "[FootContact] -> GROUND TOO HIGH (dist=" << btFabs(distanceToGround) << ", max=" << m_step_height << ")";
			// Ground is above us and too high - we are in the air
			m_foot_contact = false;
			return false;
		}

		// If ground is below us but too far, we should fall
		if (distanceToGround > m_step_height + skin) {
			CHAR_LOG(LOG_DEBUG) << "[FootContact] -> GROUND TOO LOW (dist=" << distanceToGround << ", max=" << m_step_height + skin << ")";
			// Ground is too far below - we are in the air
			m_foot_contact = false;
			return false;
		}

		// we are near the ground

		m_foot_contact_normal = results.m_hitNormalWorld;
		m_foot_contact = true;

		// update current position to be on the ground
		float correction = groundY - m_currentPosition.getY();

		// Only snap if correction is significant (> 2mm) to avoid micro-adjustments
		if (btFabs(correction) > 0.002f) {
			// Only snap if within reasonable distance
			if (btFabs(correction) <= m_step_height + skin) {
				CHAR_LOG(LOG_DEBUG) << "[FootContact] SNAP Y: " << m_currentPosition.getY() << " -> " << groundY
					<< " (correction=" << correction << ", rawGround=" << groundY << ")";
				m_currentPosition.setY(groundY);
			}
			else {
				CHAR_LOG(LOG_DEBUG) << "[FootContact] NO SNAP: correction " << btFabs(correction) << " > step_height " << m_step_height;
			}
		}
		else {
			CHAR_LOG(LOG_DEBUG) << "[FootContact] NO SNAP: correction " << btFabs(correction) << " < 0.002 (smoothed, avoiding jitter)";

		}

		CHAR_LOG(LOG_DEBUG) << "[FootContact] -> ON GROUND";
		m_move_state = moveState::GROUND;
		return true;
	}

	void BulletPhysicsCharacter::updateHeadContact()
	{
		const btScalar headClearance = 0.05f;

		const btVector3 from = m_currentPosition + m_up * (m_capsule_offset + 0.01f);
		const btVector3 to = from + m_up * headClearance;

		characterRayCallback results(m_ghost_object, from, to);
		results.m_flags |= btTriangleRaycastCallback::kF_FilterBackfaces;

		m_world->rayTest(from, to, results);

		if (!results.hasHit()) {
			m_head_contact = false;
			return;
		}

		//CHAR_DEBUG("head_contact\n");
		m_head_contact = true;
	}

	math::Vector3 BulletPhysicsCharacter::update(rhea::Position* pos, math::Vector3& move, math::Vector3& gravity)
	{
		Vector3 position = pos->getTranslation();
		btVector3 inputPos(position.x, position.y, position.z);

#if  DEBUG_CHARACTER_EVENTS || DEBUG_CHARACTER_VERBOSE
		++m_debug_tick;

		const btVector3 frameStart(
			position.x,
			position.y,
			position.z
		);
#endif

		m_currentPosition = inputPos;
		m_gravity_v = btVector3(gravity.x, gravity.y, gravity.z);

		updateHeadContact();

		switch (m_move_state) {
		case moveState::GROUND:
		{
			math::Vector3 horizontalMove = move;
			horizontalMove.y = 0.0f;

			moveStep(pos, horizontalMove);

			if (!updateFootContact()) {
				m_foot_contact = false;
				m_move_state = moveState::FALL;
			}

			break;
		}

		case moveState::FLY:
		{
			moveStep(pos, move);
			break;
		}

		case moveState::JUMP:
		case moveState::FALL:
		{
			if (sweepGravity()) {
				m_move_state = moveState::GROUND;
				break;
			}

			math::Vector3 horizontalMove = move;
			horizontalMove.y = 0.0f;

			if (horizontalMove.length() > 0.001f) {
				moveStep(pos, horizontalMove);
			}

			break;
		}
		}

		// safety check - note: if enabled this means we cant go below ground!
		if (m_check_ground)
			if (m_currentPosition.getY() < m_minimal_ground - 0.01f) {
				m_currentPosition.setY(m_minimal_ground);
				m_move_state = moveState::GROUND;
			}

		// Update ghost object position to match current position
		if (m_ghost_object) {
			btTransform ghostTransform = m_ghost_object->getWorldTransform();
			ghostTransform.setOrigin(btVector3(
				m_currentPosition.getX(),
				m_currentPosition.getY() + m_ghost_offset_y,
				m_currentPosition.getZ()
			));
			m_ghost_object->setWorldTransform(ghostTransform);
		}

		Vector3 target_position = Vector3(m_currentPosition.getX(), m_currentPosition.getY(), m_currentPosition.getZ());

#if  DEBUG_CHARACTER_EVENTS || DEBUG_CHARACTER_VERBOSE
		// Store for next frame's mismatch detection
		lastReturnedPos = m_currentPosition;
		const btVector3 frameEnd = m_currentPosition;
		const btVector3 frameDelta = frameEnd - frameStart;

		CHAR_LOG(LOG_DEBUG)
			<< "[FRAME]"
			<< " tick=" << m_debug_tick
			<< " state=" << static_cast<int>(m_move_state)
			<< " start=("
			<< frameStart.x() << ","
			<< frameStart.y() << ","
			<< frameStart.z() << ")"
			<< " end=("
			<< frameEnd.x() << ","
			<< frameEnd.y() << ","
			<< frameEnd.z() << ")"
			<< " delta=("
			<< frameDelta.x() << ","
			<< frameDelta.y() << ","
			<< frameDelta.z() << ")"
			<< " grounded=" << m_foot_contact;
#endif
		return target_position;
	}

	bool BulletPhysicsCharacter::trySlide(btConvexShape* shape,	const btVector3& startPos, const btVector3& desiredMove, btScalar hitFraction, const btVector3& hitNormal)
	{
		if (!shape)
			return false;

		const btScalar moveLength = desiredMove.length();

		if (moveLength < SIMD_EPSILON)
			return false;

		btVector3 normal = hitNormal.normalized();

		if (m_move_state == moveState::GROUND) {
			normal -= m_up * normal.dot(m_up);

			if (normal.length2() > SIMD_EPSILON) {
				normal.normalize();
			}
		}

		// small distance to the wall, so that the next sweep
		// does not immediately hit at fraction == 0.
		const btScalar skinDistance = btScalar(0.005f);

		const btScalar skinFraction = skinDistance / moveLength;

		// Move just before the first hit.
		const btScalar travelFraction = btClamped(hitFraction - skinFraction, btScalar(0.0f), btScalar(1.0f)
		);

		const btVector3 contactPos = startPos + desiredMove * travelFraction;

		// Only the remaining part is slid.
		const btVector3 remainingMove = desiredMove * (btScalar(1.0f) - hitFraction);

		// Remove the component that points into the wall.
		btVector3 slideMove = remainingMove - normal * remainingMove.dot(normal);

		// On the ground, prevent a slightly sloped
		// wall normal from moving the character up or down.
		if (m_move_state == moveState::GROUND) {
			slideMove -= m_up * slideMove.dot(m_up);
		}

		const btScalar remainingLength = remainingMove.length();

		const btScalar slideLength = slideMove.length();

		const btScalar slideDeadZone = btScalar(0.1f);

		if (remainingLength < SIMD_EPSILON || slideLength <= remainingLength * slideDeadZone)
		{
			m_currentPosition = contactPos;
			return false;
		}

		// Sliding must not move backward due to numerical errors.
		if (slideMove.dot(desiredMove) <= btScalar(0.0f)) {
			m_currentPosition = contactPos;
			return false;
		}

		const btVector3 ghostOffset(0.0f, m_ghost_offset_y,	0.0f);

		btTransform slideFrom;
		slideFrom.setIdentity();
		slideFrom.setOrigin(contactPos + ghostOffset);

		btTransform slideTo;
		slideTo.setIdentity();
		slideTo.setOrigin(contactPos + slideMove + ghostOffset);

		characterSweepCallback slideCallback(
			m_ghost_object,
			m_up,
			m_min_slope_dot
		);

		slideCallback.m_collisionFilterGroup = m_ghost_object->getBroadphaseHandle()->m_collisionFilterGroup;

		slideCallback.m_collisionFilterMask =
			m_ghost_object
			->getBroadphaseHandle()
			->m_collisionFilterMask;

		m_world->convexSweepTest(
			shape,
			slideFrom,
			slideTo,
			slideCallback
		);

		m_currentPosition = contactPos;

		if (!slideCallback.hasHit()) {
			m_currentPosition += slideMove;

#if DEBUG_CHARACTER_VERBOSE
			CHAR_LOG(LOG_DEBUG)
				<< "[SLIDE]"
				<< " tick=" << m_debug_tick
				<< " result=free"
				<< " move=("
				<< slideMove.x() << ","
				<< slideMove.y() << ","
				<< slideMove.z() << ")";
#endif

			return true;
		}

		const btScalar slideSkinFraction =
			slideLength > SIMD_EPSILON
			? skinDistance / slideLength
			: btScalar(0.0f);

		const btScalar secondFraction = btClamped(
			slideCallback.m_closestHitFraction -
			slideSkinFraction,
			btScalar(0.0f),
			btScalar(1.0f)
		);

		m_currentPosition += slideMove * secondFraction;

#if DEBUG_CHARACTER_VERBOSE
		const btVector3 secondNormal = slideCallback.m_hitNormalWorld.normalized();

		CHAR_LOG(LOG_DEBUG)
			<< "[SLIDE]"
			<< " tick=" << m_debug_tick
			<< " result=blocked"
			<< " fraction="
			<< slideCallback.m_closestHitFraction
			<< " normal=("
			<< secondNormal.x() << ","
			<< secondNormal.y() << ","
			<< secondNormal.z() << ")";
#endif

		return secondFraction > btScalar(0.0f);
	}
}
