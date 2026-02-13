#ifndef RHEA_PHYSICS_CHARACTER_H
#define RHEA_PHYSICS_CHARACTER_H

#include "Body.h"
#include <string>

namespace Rhea
{
	class PhysicsCharacterInterface
	{
	public:
		virtual ~PhysicsCharacterInterface() = default;
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

		void* queryInterface(std::string id) override {
			if (id == "PhysicsCharacter") return static_cast<PhysicsCharacterInterface*>(this);
			return PhysicsBody::queryInterface(id);
		}
	};
}

#endif // !RHEA_PHYSICS_CHARACTER_H
