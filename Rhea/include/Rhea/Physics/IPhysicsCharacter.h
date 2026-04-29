/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_PHYSICS_IPHYSICSCHARACTER_H
#define RHEA_PHYSICS_IPHYSICSCHARACTER_H

#include <Math/Vector3.h>

namespace rhea
{
	class Position;

	class IPhysicsCharacter
	{
	public:
		virtual ~IPhysicsCharacter() = default;

		// update in physics step
		virtual math::Vector3 update(Position* pos, math::Vector3& move, math::Vector3& gravity) = 0;

		// set jump_state
		virtual bool jump(math::Vector3& pos) = 0;

		// get ground state
		virtual bool isOnGround() const = 0;
	};
}

#endif // !RHEA_PHYSICS_IPHYSICSCHARACTER_H
