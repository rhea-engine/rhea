#ifndef RHEA_GRAPHICSCOMPONENT_H
#define RHEA_GRAPHICSCOMPONENT_H

#include <Rhea/Component.h>
#include "GraphicsElement.h"

namespace Rhea
{
	class GraphicsComponent : public Component
	{
		MECLASS(GraphicsComponent, Component)
	public:
		GraphicsComponent::GraphicsComponent(std::string name, std::string type_name) :
			m_element(nullptr),
			Component(name, type_name)
		{
		}
		GraphicsComponent(GraphicsElement* element) :
			m_element(element),
			Component("graphics", "//meworla/graphicscomponent")
		{}
		~GraphicsComponent() { if (m_element) delete m_element; }

		void setElement(GraphicsElement* element) { m_element = element; }
		GraphicsElement* getElement() { return m_element; }

	protected:
		GraphicsElement* m_element;
	};
}
#endif // !RHEA_GRAPHICSCOMPONENT_H
