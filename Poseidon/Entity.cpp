#include "Entity.h"
#include "Viewport.h"

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

	bool Entity::render(Viewport* viewport, Camera* camera)
	{
		if (m_mesh) {


			Poseidon::Matrix3x4 model_wtm = get_position()->getWorldTransform();
			Poseidon::Matrix4 viewproj = camera->getProjectionMatrixRS() * camera->getViewMatrix();

			// set up GPU input
			cbPerObjMVP.vp = viewproj;
			cbPerObjMVP.model = model_wtm;
			viewport->get_renderer()->getRenderer()->setGPUParameter((void *)&cbPerObjMVP);

			// set material
			viewport->get_renderer()->getRenderer()->setMaterial(m_mesh->getTexture()->get_texture());

			// draw the entity

			// bind the vertex and index buffer
			Hippios::VertexBuffer* vb = m_mesh->get_vbuffer();
			Hippios::IndexBuffer* ib = m_mesh->get_ibuffer();
			viewport->get_renderer()->update(vb, ib);
			viewport->get_renderer()->getRenderer()->drawIndexed(36, 0, 0);

			return false;
		}
		return false;
	}
}