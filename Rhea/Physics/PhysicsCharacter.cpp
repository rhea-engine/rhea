#include "PhysicsCharacter.h"

namespace Rhea {

	Poseidon::Vector3 PhysicsCharacter::update(Poseidon::Position* pos, Poseidon::Vector3& move, Poseidon::Vector3& gravity)
	{
		Poseidon::Vector3 position = pos->get_worldspace();

		Poseidon::Vector3 trans = pos->get_rotation() * move;

		Poseidon::Vector3 target_position = position + Poseidon::Vector3(trans.x, trans.y, trans.z);

		return target_position;
	}
	
	bool PhysicsCharacter::jump(Poseidon::Vector3& pos)
	{
		return false;
	}
}