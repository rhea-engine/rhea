#ifndef RHEA_PHYSICS_BULLET_CHARACTER_H
#define RHEA_PHYSICS_BULLET_CHARACTER_H

#include <vector>

#include <Poseidon/Position.h>
#include <bullet/src/LinearMath/btGeometryUtil.h>
#include <bullet/src/LinearMath/btQuaternion.h>

#include "../Body.h"

class btDynamicsWorld;
class btPairCachingGhostObject;
class btCollisionWorld;
class btIDebugDraw;
class btKinematicCharacterController;

namespace Rhea
{
	class Item;
	class Terrain;
	class BulletPhysicsGhost;

	class BulletPhysicsCharacter : public PhysicsBody
	{
	public:
		BulletPhysicsCharacter(btDynamicsWorld *world, Item* item);
		~BulletPhysicsCharacter();

		void init();

		// can we controll the move?
		bool isMoveControllable();

		// set position
		void setPosition(Poseidon::Vector3& position);

		// set jump_state
		bool jump(Poseidon::Vector3& pos);
		// set flying
		void fly();

		// set orientation
		void setOrientation(Poseidon::Quaternion& rot);	
		
		// stop the current movement
		void stop();

		// get ground state
		bool isOnGround();

		// reduce gravity from current position
		void applyGravity();

		// detect all contacts the ghost object has currently
		void detectGhostContacts();

		// move in the given direction - move must be already adjusted to timestep
		void moveStep(Poseidon::Position* pos, Poseidon::Vector3& move);

		// check if we are on the floor
		// if we are not but in step_height distance adjust current position
		// ccd - make sure didnt pass the ground in the last step
		bool updateFootContact(bool ccd = false);

		// check if we bump on the ceiling
		void updateHeadContact();

		// update in physics step
		Poseidon::Vector3 update(Poseidon::Position* pos, Poseidon::Vector3& move, Poseidon::Vector3& gravity);

	protected:
		enum class moveState {
			GROUND,
			FALL,
			FLY,
			JUMP,
		};
		
		float m_gravity;
		moveState m_move_state;
		Poseidon::Vector3 m_forced_movement;
		Poseidon::Position* m_position;

		btDynamicsWorld* m_world;
		Item* m_item;
		BulletPhysicsGhost* m_ghost;
		btPairCachingGhostObject* m_ghost_object;

		float m_capsule_offset;
		float m_step_height;
		float m_height;
		float m_radius;
		float m_levitation;

		bool m_foot_contact;
		bool m_head_contact;
		btVector3 m_foot_contact_pos;
		btVector3 m_foot_contact_normal;
		btVector3 m_head_contact_pos;
		btVector3 m_gravity_v;
		btVector3 m_last_position;

		bool m_check_ground;
		float m_minimal_ground;

		bool m_hitting_wall;
		std::vector<Poseidon::Vector3> m_surface_hits_normals;

		//some internal variables
		btVector3 m_currentPosition;
		btVector3 m_targetPosition;
	};
}

#endif // !RHEA_PHYSICS_BULLET_CHARACTER_H
