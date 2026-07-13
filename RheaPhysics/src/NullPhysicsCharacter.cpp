/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "NullPhysicsCharacter.h"

namespace rhea::physics {

	math::Vector3 NullPhysicsCharacter::update(rhea::Position* pos, math::Vector3& move, math::Vector3& gravity)
	{
		math::Vector3 position = pos->getTranslation();

		math::Vector3 trans = pos->getRotation() * move;

		math::Vector3 target_position = position + math::Vector3(trans.x, trans.y, trans.z);

		return target_position;
	}
	
	bool NullPhysicsCharacter::jump(math::Vector3& pos)
	{
		return false;
	}
}