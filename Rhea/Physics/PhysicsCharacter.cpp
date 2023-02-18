#include "PhysicsCharacter.h"

namespace Rhea {

	Math::Vector3 PhysicsCharacter::update(Rhea::Position* pos, Math::Vector3& move, Math::Vector3& gravity)
	{
		Math::Vector3 position = pos->get_worldspace();

		Math::Vector3 trans = pos->get_rotation() * move;

		Math::Vector3 target_position = position + Math::Vector3(trans.x, trans.y, trans.z);

		return target_position;
	}
	
	bool PhysicsCharacter::jump(Math::Vector3& pos)
	{
		return false;
	}
}