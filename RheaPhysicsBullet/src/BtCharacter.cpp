#include "BtCharacter.h"
#include <Rhea/Core/Item.h>
#include "BtGhost.h"

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

			btScalar dotUp = m_up.dot(hitNormalWorld);
			if (dotUp < m_minSlopeDot)
			{
				return btScalar(1.0);
			}

			return ClosestConvexResultCallback::addSingleResult(convexResult, normalInWorldSpace);
		}

	protected:
		btCollisionObject* m_me;
		const btVector3 m_up;
		btScalar m_minSlopeDot;
	};

	BulletPhysicsCharacter::BulletPhysicsCharacter(btDynamicsWorld* world, rhea::Item* item, const rhea::PhysicsBodyDesc& desc) :
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
		BulletPhysicsBody(world, item, desc)
	{
		// prepare default values
		m_step_height = item->getValue("step_height", 0.3f);
		m_height = item->getValue("collision_shape_height", 1.4f);
		m_radius = item->getValue("collision_shape_radius", 0.7f);

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
		m_gravity = item->getValue("gravity", -9.8f);
		m_capsule_offset = m_height * 0.5f + m_levitation;

		float ghost_offset_y = m_step_height + m_radius + m_height / 2.0f;
		item->setValue("collision_shape_height", m_height);
		item->setValue("collision_shape_radius", m_radius);
		item->setValue("collision_shape_offset_y", ghost_offset_y);

		// minimal y postion check
		m_check_ground = item->getValue("ground_check", true);
		m_minimal_ground = item->getValue("ground_height", 0.0f);

		// create the ghost object
		m_ghost = new BulletPhysicsGhost(world, item, desc);
	}

	bool BulletPhysicsCharacter::init()
	{
		// init ghost object
		if (m_ghost->init()) {
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
	
	void BulletPhysicsCharacter::applyGravity()
	{
		m_currentPosition += m_gravity_v;
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
		Vector3 trans = pos->get_rotation() * move;
		//Vector3 position = pos->get_worldspace();

		m_targetPosition = m_currentPosition + btVector3(trans.x, trans.y, trans.z);
		
		if (m_hitting_wall) {
			// resolve the collision with the wall (or the too steep slope)

			for (unsigned int i = 0, size = (unsigned int)m_surface_hits_normals.size(); i < size; i++)
			{
				// we hit something

				// apply corection using the hit normals
				Vector3 trans_c = m_surface_hits_normals[i];
				m_targetPosition -= (btVector3(trans_c.x, trans_c.y, trans_c.z) * 1.05f *move.length());
/*				printf("move: (%f, %f, %f) normal:(%f, %f, %f) trans_c:(%f, %f, %f), length_move:%f\n",
					trans.x, trans.y, trans.z,
					m_surface_hits_normals[i].x, m_surface_hits_normals[i].y, m_surface_hits_normals[i].z,
					trans_c.x, trans_c.y, trans_c.z,
					move.length()
				);
*/
			}

		}

		m_currentPosition = m_targetPosition;		
	}

	bool BulletPhysicsCharacter::updateFootContact(bool ccd)
	{
		//		Vector3 pFrom = m_position->get_worldspace();
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
					m_foot_contact_pos = results.m_hitPointWorld;
					m_foot_contact_normal = results.m_hitNormalWorld;
					m_foot_contact = true;
					m_currentPosition.setY(m_foot_contact_pos.getY());
					//printf("touchdown\n");
					return true;
				}
			}
			m_foot_contact = false;
			return false;
		}

		// we are near the ground
		// m_closestHitFraction < 0.5 means we have to climb
		// > 0.5 means we have to descend

		m_move_state = moveState::GROUND;

		if (results.m_closestHitFraction == 0.5f) {
			// exactly on ground, prevent jittering
			return true;
		}

#if 0
		if (results.m_closestHitFraction < 0.5f)
			printf("foot_contact\n");
		else if (results.m_closestHitFraction < 0.5f)
			printf("slope\n");
#endif
		m_foot_contact_pos = results.m_hitPointWorld;
		m_foot_contact_normal = results.m_hitNormalWorld;
		m_foot_contact = true;
		m_currentPosition.setY(m_foot_contact_pos.getY());
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
		Vector3 position = pos->get_worldspace();
		m_currentPosition = btVector3(position.x, position.y, position.z);
		m_gravity_v = btVector3(gravity.x, gravity.y, gravity.z);

		Vector3 dir = pos->get_rotation() * move;

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
		case moveState::FALL:
			applyGravity();
			// if on ground reset movement state - need ccd because velocity may be high
			if (updateFootContact(true))
				m_move_state = moveState::GROUND;
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