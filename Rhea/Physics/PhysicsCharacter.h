#ifndef RHEA_PHYSICS_CHARACTER_H
#define RHEA_PHYSICS_CHARACTER_H

#include "Body.h"

namespace Rhea
{
	class PhysicsCharacterInterface
	{
	public:
		// update in physics step
		virtual Math::Vector3 update(Position* pos, Math::Vector3& move, Math::Vector3& gravity) = 0;

		// set jump_state
		virtual bool jump(Math::Vector3& pos) = 0;

		// get ground state
		virtual bool isOnGround() {
			return true;
		}
	};

	class PhysicsCharacter : public PhysicsBody, public PhysicsCharacterInterface
	{
	public:
		PhysicsCharacter(Item* item) :
			PhysicsBody(item)
		{
		}

		// update in physics step
		Math::Vector3 update(Position* pos, Math::Vector3& move, Math::Vector3& gravity) override;
		bool jump(Math::Vector3& pos) override;
	};
}

#endif // !RHEA_PHYSICS_CHARACTER_H
