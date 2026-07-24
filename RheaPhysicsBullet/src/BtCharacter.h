/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_PHYSICS_BULLET_CHARACTER_H
#define RHEA_PHYSICS_BULLET_CHARACTER_H

#include <vector>

#include <Rhea/Core/Position.h>
#include <bullet/src/LinearMath/btGeometryUtil.h>
#include <bullet/src/LinearMath/btQuaternion.h>

#include <Rhea/Physics/IPhysicsCharacter.h>
#include "BtBody.h"
#include <Rhea/Physics/PhysicsBodyDescriptor.h>

 // Set to 1 to enable detailed character controller debug output
#define DEBUG_CHARACTER_EVENTS 0
#define DEBUG_CHARACTER_VERBOSE 0

class btDynamicsWorld;
class btPairCachingGhostObject;
class btCollisionWorld;
class btIDebugDraw;
class btKinematicCharacterController;
class btConvexShape;

namespace rhea
{
	class Item;
	class Terrain;
}

namespace rhea::physics::bullet
{
	class BulletPhysicsGhost;

	class BulletPhysicsCharacter : public BulletPhysicsBody, public rhea::IPhysicsCharacter
	{
	public:
		BulletPhysicsCharacter(btDynamicsWorld *world, rhea::Item* item);
		~BulletPhysicsCharacter();

		bool init() override;

		// can we controll the move?
		bool isMoveControllable() const override;

		// set position
		void setPosition(const math::Vector3& position) override;

		// set jump_state
		bool jump(math::Vector3& pos) override;
		// set flying
		void fly();

		// set orientation
		void setOrientation(const math::Quaternion& rot) override;	
		
		// stop the current movement
		void stop() override;

		// get ground state
		bool isOnGround() const override;

		// capsule sweep to detect if we are on the ground and move gavity to the ground if we are not
		bool sweepGravity();

		// move in the given direction - move must be already adjusted to timestep
		void moveStep(rhea::Position* pos, math::Vector3& move);

		// check if we are on the floor
		// if we are not but in step_height distance adjust current position
		bool updateFootContact();

		// check if we bump on the ceiling
		void updateHeadContact();

		// update in physics step
		math::Vector3 update(rhea::Position* pos, math::Vector3& move, math::Vector3& gravity) override;

		void* queryInterface(const std::string& id) override {
			if (id == "PhysicsCharacter") return static_cast<rhea::IPhysicsCharacter*>(this);
			return rhea::PhysicsBody::queryInterface(id);
		}

	protected:
		enum class moveState {
			GROUND,
			FALL,
			FLY,
			JUMP,
		};

		bool trySlide(btConvexShape* shape, const btVector3& startPos, const btVector3& desiredMove, btScalar hitFraction, const btVector3& hitNormal);
		
		moveState m_move_state;

		btDynamicsWorld* m_world;
		BulletPhysicsGhost* m_ghost = nullptr;
		btPairCachingGhostObject* m_ghost_object;
		float m_ghost_offset_y = 0.0f;

		float m_capsule_offset = 0.0f;
		float m_step_height = 0.0f;
		float m_height = 0.0f;
		float m_radius = 0.0f;
		
		btVector3 m_up = btVector3(0.0f, 1.0f, 0.0f);
		btScalar m_min_slope_dot = 0.707f;

		bool m_foot_contact;
		bool m_head_contact;
		btVector3 m_foot_contact_normal;
		btVector3 m_gravity_v;

		bool m_check_ground = false;
		float m_minimal_ground = 0.0f;

		// the current position of the character - used to detect if we are on the ground or not
		btVector3 m_currentPosition;

		// debug info
#if DEBUG_CHARACTER_EVENTS || DEBUG_CHARACTER_VERBOSE
		uint64_t m_debug_tick = 0;
		btVector3 m_debug_frame_start;
#endif
	};
}

#endif // !RHEA_PHYSICS_BULLET_CHARACTER_H
