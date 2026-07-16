/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_PHYSICS_COMPONENT_H
#define RHEA_PHYSICS_COMPONENT_H

#include <Rhea/Component.h>
#include <Rhea/Components/MovementListenerComponent.h>
#include <Rhea/Physics/PhysicsBody.h>
#include <Rhea/Core/Item.h>

namespace rhea::physics
{
	class PhysicsComponent : public rhea::MovementListenerComponent
	{
		RECLASS(PhysicsComponent, rhea::MovementListenerComponent, "//meworla/physicscomponent")
	public:
		PhysicsComponent(std::string name, std::string type_name) :
			rhea::MovementListenerComponent(name, type_name)
		{
		}
		PhysicsComponent() :
			rhea::MovementListenerComponent("physics", "//meworla/physicscomponent")
		{}
		~PhysicsComponent() { 
			// ensure cache cleared (if still pointing to our body)
			if (auto* p_item = doCast<rhea::Item>(getParent())) {
				if (p_item->getPhysicsBody() == m_body.get())
					p_item->setPhysicsBody(nullptr);
			}
		}

		void setBody(std::unique_ptr<rhea::PhysicsBody> body) { 
			m_body = std::move(body);
			if (auto* p_item = doCast<rhea::Item>(getParent())) {
				if (m_body)
					p_item->setPhysicsBody(m_body.get());
			}
		}
		rhea::PhysicsBody* getBody() { return m_body.get(); }
		void deleteBody() { 
			if (auto* item = doCast<rhea::Item>(getParent())) {
				if (item->getPhysicsBody() == m_body.get())
					item->setPhysicsBody(nullptr);
			}

			m_body.reset();
		}

		// movement listener functions
		void onMovementChanged(const math::Vector3& move) override {
			if (m_body)
				m_body->move(move);
		}
		void onTurnChanged(float turn) override {
			if (m_body)
				m_body->turn(turn);
		}
		void onTransformChanged() override {}

	protected:
		std::unique_ptr<rhea::PhysicsBody> m_body;

	};
}

#endif // !RHEA_PHYSICS_COMPONENT_H
