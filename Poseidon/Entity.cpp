#include "Entity.h"
#include "Viewport.h"
#include "RenderQueue.h"
#include "Camera.h"

#include "Hippios/Renderer.h"

namespace Poseidon
{
	Entity::Entity()
	{
		createPosition(0);
	}

	void Entity::set_mesh(Mesh* node)
	{
		m_mesh = node;
	}

	bool Entity::onUpdate()
	{
		return false;
	}
	
	void Entity::cullPhase(RenderQueue* queue, Camera* camera)
	{
		// TODO - really cull
		if (!m_mesh)
			return;

		queue->addRenderable(this);
	}

	void Entity::getRenderInput(Hippios::VertexBuffer** pvb, Hippios::IndexBuffer** pib)
	{
		*pvb = m_mesh->get_vbuffer();
		*pib = m_mesh->get_ibuffer();
	}

	Texture* Entity::getMaterial()
	{
		return m_mesh->getTexture();
	}
}