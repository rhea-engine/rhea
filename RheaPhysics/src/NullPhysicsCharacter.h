/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_NULLPHYSICS_CHARACTER_H
#define RHEA_NULLPHYSICS_CHARACTER_H

#include "NullBody.h"
#include <string>
#include <Rhea/Physics/IPhysicsCharacter.h>

namespace rhea::physics
{
	class NullPhysicsCharacter : public NullPhysicsBody, public rhea::IPhysicsCharacter
	{
	public:
		NullPhysicsCharacter(rhea::Item* item) :
			NullPhysicsBody(item)
		{
		}

		// update in physics step
		math::Vector3 update(rhea::Position* pos, math::Vector3& move, math::Vector3& gravity) override;
		bool jump(math::Vector3& pos) override;

		virtual bool isOnGround() const override { return true; }

		void* queryInterface(const std::string& id) override {
			if (id == "PhysicsCharacter") return static_cast<IPhysicsCharacter*>(this);
			return PhysicsBody::queryInterface(id);
		}
	};
}

#endif // !RHEA_NULLPHYSICS_CHARACTER_H
