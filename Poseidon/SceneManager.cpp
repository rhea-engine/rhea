#include "SceneManager.h"
#include "Node.h"
#include "Viewport.h"
#include "Renderable.h"
#include "RenderQueue.h"
#include "RenderContext.h"
#include "Texture.h"
#include "Material.h"
#include "Shader.h"
#include "Mesh.h"

#include <Hippios/Renderer.h>
#include <Hippios/GraphicsBuffer.h>
#include <Rhea/Fabrics/Core/Material.h>
#include <Rhea/Graphics/MaterialManager.h>
#include <Rhea/Graphics/MeshElement.h>
#include <Rhea/Fabrics/3D/Mesh.h>

namespace Poseidon
{
	SceneManager::SceneManager() :
		m_renderer(nullptr),
		m_current_material(nullptr),
		m_frame_buffer(nullptr),
		m_object_buffer(nullptr)
	{
		m_rend_queue = new RenderQueue();
	}

	SceneManager::~SceneManager()
	{
		if (m_frame_buffer)
			delete m_frame_buffer;
		if (m_object_buffer)
			delete m_object_buffer;
	}

	Node* SceneManager::get_root()
	{
		if (!m_tree) {
			m_tree = createNode();
		}
		return m_tree;
	}

	void SceneManager::setRenderer(Hippios::Renderer* renderer)
	{
		m_renderer = renderer;

		//initialize graphic buffers
		m_frame_buffer = m_renderer->createGraphicsBuffer(sizeof(cbPerFrame));
		m_renderer->bindGPUBuffer(m_frame_buffer, 0, "VS");
		m_object_buffer = m_renderer->createGraphicsBuffer(sizeof(cbPerObject));
		m_renderer->bindGPUBuffer(m_object_buffer, 1, "VS");
	}

	Node* SceneManager::createNode()
	{
		Node* node = new Node();
		return node;
	}

	bool SceneManager::update()
	{
		return false;
	}

	bool SceneManager::render()
	{
		// iterate over all viewports and execute the phases per viewport
		for (std::vector<Viewport*>::iterator it = m_viewports.begin(); it != m_viewports.end(); ++it) {
			Viewport* viewport = *it;

			// clear pass
			m_renderer->clearPass();

			// iterate over render contexts of this viewport
			std::vector<RenderContext*> r_contexts = viewport->getRenderContextList();
			for (std::vector<RenderContext*>::iterator rc_it = r_contexts.begin(); rc_it != r_contexts.end(); ++rc_it) {
				RenderContext* context = *rc_it;

				// cull pass	
				m_rend_queue->clear();
				context->cullObjects(m_rend_queue);

				// render pass
				Camera* camera = context->get_camera();
				if (!camera)
					continue;
				m_renderer->setStencilState(context->getStencilState());

				// fill per frame buffer
				per_frame_buf.v = camera->getViewMatrix();
				per_frame_buf.p = camera->getProjectionMatrixRS();
				per_frame_buf.vp = per_frame_buf.p * per_frame_buf.v;

				m_frame_buffer->update((void*)&per_frame_buf);

				Item* renderable = m_rend_queue->first();
				for (Item* renderable = m_rend_queue->first(); renderable != NULL; renderable = m_rend_queue->next()) {

					Rhea::RenderComponent* r_comp = renderable->getGraphicsComponent();

					// material
					Rhea::Mesh* rmesh = r_comp->getMesh();
					if (!rmesh)
						continue;
					Rhea::Material* rmat = rmesh->getMaterial();
					//Rhea::Material* rmat = r_comp->getMaterial(); //rmesh->getMaterial();					
					Material* mat = nullptr;
					if (rmat) {
						Rhea::GraphicsMaterial* gmat = (Rhea::GraphicsMaterial*)rmat->getElement("graphics");
						if (gmat)
							mat = gmat->m_material;
					}
					if (mat != m_current_material && mat) {
						m_current_material = mat;
						mat->activate();
					}
					if (!m_current_material) {
						printf("SceneManager::render: No shader found\n");
						return true;
					}

					// fill per object buffer
					Rhea::Transform* transform = renderable->getTransform();
					Position* position = nullptr;
					if (transform)
						position = transform->get_derived_position();

					// check if renderable has a position
					if (position) {
						Math::Matrix3x4 model_wtm = position->getWorldTransform();
						per_obj_buf.model = model_wtm;
						m_object_buffer->update((void*)&per_obj_buf);
					}

					// set texture(s)

					std::vector<Poseidon::Texture*> texs = m_current_material->getTextures();
					if (texs.size()) {
						for (int i = 0; i < texs.size(); i++) {
							m_renderer->setTexture(texs[i]->get_texture(), i);
						}
					}

					// draw the entity

					// bind the vertex and index buffer
					Hippios::VertexBuffer* vb;
					Hippios::IndexBuffer* ib;
					int num_indices;
					
					Rhea::GraphicsMesh* m_elem = (Rhea::GraphicsMesh*)rmesh->getElement("graphics");
					if (!m_elem)
						continue;
					m_elem->m_mesh->getRenderInput(&vb, &ib, num_indices);

					if (m_current_material->getShader()->getVertexType() == 0) {
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

	Viewport* SceneManager::getViewport(std::string name)
	{
		// search viewport with that name
		for (std::vector<Viewport*>::iterator it = m_viewports.begin(); it != m_viewports.end(); ++it) {
			Viewport* viewport = *it;
			if (viewport->isName(name))
				return viewport;
		}
		return nullptr;
	}

}