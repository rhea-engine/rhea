#ifndef RHEA_PHYSICS_CHARACTER_H
#define RHEA_PHYSICS_CHARACTER_H

#include "Body.h"

namespace Rhea
{
	class PhysicsCharacter : public PhysicsBody
	{
	public:
		PhysicsCharacter(Item* item) :
			PhysicsBody(item)
		{}

		// update in physics step
		virtual Math::Vector3 update(Position* pos, Math::Vector3& move, Math::Vector3& gravity);

		// set jump_state
		virtual bool jump(Math::Vector3& pos);

		// get ground state
		virtual bool isOnGround() {
			return true;
		}
	};
}

#endif // !RHEA_PHYSICS_CHARACTER_H
