/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_PHYSICS_BODY_H
#define RHEA_PHYSICS_BODY_H

#include <string>
#include <Math/Vector3.h>
#include <Math/Quaternion.h>

namespace rhea
{
	class Item;

	class PhysicsBody
	{
	public:
		explicit PhysicsBody(Item* item) : m_item(item) {}
		virtual ~PhysicsBody() = default;

		// move by vector
		virtual void move(math::Vector3 move) {};

		// turn by radian angle
		virtual void turn(float turn) {};

		// High-level commands: keep virtual so backends implement
		virtual void setPosition(const math::Vector3& position) = 0;
		virtual void setOrientation(const math::Quaternion& rot) {}
		virtual void applyOrientation(const math::Quaternion& rot) {}

		virtual void stop() {}

		virtual void DisableMassUpdate() {}
		virtual void EnableMassUpdate() {}
		virtual void applyImpulse(const math::Vector3& impulse) {}
		virtual void setAngularVelocity(const math::Vector3& velocity) {}

		virtual bool isMoveControllable() const { return true; }
		bool isKinematic() const { return m_isKinematic; }
		bool isDynamic() const { return !m_isKinematic && !m_isStatic; }

		virtual void* queryInterface(const std::string& interfaceId) { return nullptr; }

		template <typename T>
		T* query(const std::string& id) { return static_cast<T*>(queryInterface(id)); }

	protected:
		Item* m_item = nullptr;
		bool m_isKinematic = false;
		bool m_isStatic = false;
	};
}


#endif // !RHEA_PHYSICS_BODY_H