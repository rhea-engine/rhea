#include "Poseidon/Entity.h"
#include "Poseidon/Viewport.h"
#include "Poseidon/RenderQueue.h"
#include "Poseidon/Camera.h"

#include "Hippios/Renderer.h"

namespace Poseidon
{
	Entity::Entity() :
		m_hidden(false),
		m_vertex_size(0),
		m_material(nullptr)
	{ }

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
		// make sure we have geometry
		if (!m_mesh)
			return;

		// if we are hidden we dont add ourself or our children to the list
		if (m_hidden)
			return;

		// TODO - really cull based on AABB or similar

		// add myself to the render list
		//queue->addRenderable(this);

		// add children to render list
		for (auto it = m_groups["renderable"].begin(); it != m_groups["renderable"].end(); ++it)
		{
			Renderable* child = static_cast<Renderable*>(*it);
			child->cullPhase(queue, camera);
		}		
	}

	void Entity::getRenderInput(size_t mesh_idx, Hippios::VertexBuffer** pvb, Hippios::IndexBuffer** pib, size_t& num_indices)
	{
		if (m_mesh) {
			*pvb = m_mesh->get_vbuffer();
			*pib = m_mesh->get_ibuffer(mesh_idx);
			num_indices = m_mesh->getNumIndices(mesh_idx);
		}
		else {
			*pvb = NULL;
			*pib = NULL;
			num_indices = 0;
		}
	}

	Material* Entity::getMaterial()
	{
		return m_material;
	}

	void Entity::setMaterial(Material* material)
	{
		m_material = material;
	}	
}