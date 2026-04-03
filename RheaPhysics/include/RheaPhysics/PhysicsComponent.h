#ifndef RHEA_PHYSICS_COMPONENT_H
#define RHEA_PHYSICS_COMPONENT_H

#include <Rhea/Component.h>
#include <Rhea/Components/MovementListenerComponent.h>
#include <Rhea/Physics/PhysicsBody.h>
#include <Rhea/Core/Item.h>

namespace Rhea
{
	class PhysicsComponent : public MovementListenerComponent
	{
		RECLASS(PhysicsComponent, MovementListenerComponent, "//meworla/physicscomponent")
	public:
		PhysicsComponent(std::string name, std::string type_name) :
			MovementListenerComponent(name, type_name)
		{
		}
		PhysicsComponent() :
			MovementListenerComponent("physics", "//meworla/physicscomponent")
		{}
		~PhysicsComponent() { 
			// ensure cache cleared (if still pointing to our body)
			if (auto* p_item = doCast<Item>(getParent())) {
				if (p_item->getPhysicsBody() == m_body.get())
					p_item->setPhysicsBody(nullptr);
			}
		}

		void setBody(std::unique_ptr<PhysicsBody> body) { 
			m_body = std::move(body);
			if (auto* p_item = doCast<Item>(getParent())) {
				if (m_body)
					p_item->setPhysicsBody(m_body.get());
			}
		}
		PhysicsBody* getBody() { return m_body.get(); }

		// movement listener functions
		void onMovementChanged(const Math::Vector3& move) override {
			if (m_body)
				m_body->move(move);
		}
		void onTurnChanged(float turn) override {
			if (m_body)
				m_body->turn(turn);
		}
		void onPositionChanged() override {}

	protected:
		std::unique_ptr<PhysicsBody> m_body;

	};
}

#endif // !RHEA_PHYSICS_COMPONENT_H
