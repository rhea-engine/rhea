#include "SceneManager.h"
#include "Node.h"
#include "Viewport.h"
#include "Renderable.h"
#include "RenderQueue.h"
#include "Texture.h"
#include "Material.h"

namespace Poseidon
{
	SceneManager::SceneManager() :
		m_renderer(nullptr),
		m_current_material(nullptr)
	{
		m_rend_queue = new RenderQueue();
	}

	Node* SceneManager::get_root()
	{
		if (!m_tree) {
			m_tree = createNode();
		}
		return m_tree;
	}

	Node* SceneManager::createNode()
	{
		Node* node = new Node();
		return node;
	}

	bool SceneManager::update()
	{
		if (m_tree)
			return m_tree->update();
		else
			return false;
	}

	bool SceneManager::render()
	{
		// iterate over all viewports an execute the phases per viewport
		for (std::vector<Viewport*>::iterator it = m_viewports.begin(); it != m_viewports.end(); ++it) {
			Viewport* viewport = *it;

			// clear pass
			m_renderer->clearPass();
			m_rend_queue->clear();

			// cull pass	
			viewport->cullObjects(m_rend_queue);

			// render pass
			Camera* camera = viewport->get_camera();
			if (!camera)
				return false;
			Poseidon::Matrix4 viewproj = camera->getProjectionMatrixRS() * camera->getViewMatrix();

			Renderable* renderable = m_rend_queue->first();
			for (Renderable* renderable = m_rend_queue->first(); renderable != NULL; renderable = m_rend_queue->next()) {

				// material
				Material* mat = renderable->getMaterial();
				if (mat != m_current_material && mat) {
					m_current_material = mat;
					mat->activate();
				}
				if (!m_current_material) {
					printf("No shader found\n");
					return true;
				}

				if (m_current_material->getType() == 1) {
					Poseidon::Matrix3x4 model_wtm = renderable->get_derived_position()->getWorldTransform();

					// set up GPU input
					cbPerObjMVP.vp = viewproj;
					cbPerObjMVP.model = model_wtm;
					viewport->get_renderer()->getRenderer()->setGPUParameter((void*)&cbPerObjMVP);
				}
				else {
					// set up GPU input
					cbPerObj.vp = viewproj;
					viewport->get_renderer()->getRenderer()->setGPUParameter((void*)&cbPerObj);
				}

				// set texture
				Poseidon::Texture* tex = renderable->getTexture();
				if (tex)
					m_renderer->setTexture(tex->get_texture());

				// draw the entity

				// bind the vertex and index buffer
				Hippios::VertexBuffer* vb;
				Hippios::IndexBuffer* ib;
				int num_indices;
				renderable->getRenderInput(&vb, &ib, num_indices);

				if (m_current_material->getType() == 0) {
					if (vb) {
						m_renderer->bindBuffers(vb, ib);
						m_renderer->draw(num_indices, 0);
					}
				}
				else {
					if (vb && num_indices) {
						m_renderer->bindBuffers(vb, ib);
						m_renderer->drawIndexed(num_indices, 0, 0);
					}
				}
			}

			// present pass
			if (m_renderer->presentPass())
				return true;
		}
		return false;
	}

	void SceneManager::registerViewport(Viewport* viewport)
	{
		m_viewports.push_back(viewport);
	}

	void SceneManager::deregisterViewport(Viewport* viewport)
	{
		m_viewports.erase(std::remove(m_viewports.begin(), m_viewports.end(), viewport), m_viewports.end());
	}

	Viewport* SceneManager::getViewport()
	{
		// return root as viewprot
		return static_cast<Viewport*>(m_tree);
	}

}