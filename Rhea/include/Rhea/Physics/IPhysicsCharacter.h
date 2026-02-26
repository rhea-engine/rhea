#ifndef RHEA_PHYSICS_IPHYSICSCHARACTER_H
#define RHEA_PHYSICS_IPHYSICSCHARACTER_H

#include <Math/Vector3.h>

namespace Rhea
{
	class Position;

	class IPhysicsCharacter
	{
	public:
		virtual ~IPhysicsCharacter() = default;

		// update in physics step
		virtual Math::Vector3 update(Position* pos, Math::Vector3& move, Math::Vector3& gravity) = 0;

		// set jump_state
		virtual bool jump(Math::Vector3& pos) = 0;

		// get ground state
		virtual bool isOnGround() const = 0;
	};
}

#endif // !RHEA_PHYSICS_IPHYSICSCHARACTER_H
