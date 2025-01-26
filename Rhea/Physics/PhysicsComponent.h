#ifndef RHEA_PHYSICS_COMPONENT_H
#define RHEA_PHYSICS_COMPONENT_H

#include <Rhea/Component.h>
#include "Body.h"

namespace Rhea
{
	class PhysicsComponent : public Component
	{
		RECLASS(PhysicsComponent, Component, "//meworla/physicscomponent")
	public:
		PhysicsComponent::PhysicsComponent(std::string name, std::string type_name) :
			m_body(nullptr),
			Component(name, type_name)
		{
		}
		PhysicsComponent(PhysicsBody* body) :
			m_body(body),
			Component("physics", "//meworla/physicscomponent")
		{}
		~PhysicsComponent() { if (m_body) delete m_body; }

		void setBody(PhysicsBody* body) { m_body = body; }
		PhysicsBody* getBody() { return m_body; }

	protected:
		PhysicsBody* m_body;

	};
}

#endif // !RHEA_PHYSICS_COMPONENT_H
