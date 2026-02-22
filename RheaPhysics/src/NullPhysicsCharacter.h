#ifndef RHEA_NULLPHYSICS_CHARACTER_H
#define RHEA_NULLPHYSICS_CHARACTER_H

#include "NullBody.h"
#include <string>
#include <Rhea/Physics/IPhysicsCharacter.h>

namespace Rhea
{
	class NullPhysicsCharacter : public NullPhysicsBody, public IPhysicsCharacter
	{
	public:
		NullPhysicsCharacter(Item* item) :
			NullPhysicsBody(item)
		{
		}

		// update in physics step
		Math::Vector3 update(Position* pos, Math::Vector3& move, Math::Vector3& gravity) override;
		bool jump(Math::Vector3& pos) override;

		virtual bool isOnGround() const override { return true; }

		void* queryInterface(const std::string& id) override {
			if (id == "PhysicsCharacter") return static_cast<IPhysicsCharacter*>(this);
			return PhysicsBody::queryInterface(id);
		}
	};
}

#endif // !RHEA_NULLPHYSICS_CHARACTER_H
