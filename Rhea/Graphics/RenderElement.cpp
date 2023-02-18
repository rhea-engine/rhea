#include "RenderElement.h"
#include "Poseidon/Entity.h"
#include "Poseidon/RenderContext.h"

namespace Rhea
{
	GraphicsRenderElement::~GraphicsRenderElement()
	{
		if (m_entity) {
			if (m_render_context)
				m_render_context->removeRenderChild(static_cast<Poseidon::Entity*>(m_entity));
			delete m_entity;
		}
	}

	void GraphicsRenderElement::setPosition(Rhea::Position& position)
	{
		//m_entity->get_position()->setPosition(position);
	}

	void GraphicsRenderElement::setPosition(Math::Vector3 position)
	{
		//m_entity->get_position()->setPosition(position);
	}

	void GraphicsRenderElement::lookAt(Math::Vector3 look_at)
	{
		// m_entity->get_position()->lookAt(look_at);
	}
}