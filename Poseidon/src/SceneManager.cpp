#define NOMINMAX
#include "Poseidon/SceneManager.h"
#include "Poseidon/Node.h"
#include "Poseidon/Viewport.h"
#include "Poseidon/Renderable.h"
#include "Poseidon/RenderQueue.h"
#include "Poseidon/RenderContext.h"
#include "Poseidon/Texture.h"
#include "Poseidon/Material.h"
#include "Poseidon/Shader.h"
#include "Poseidon/Mesh.h"
#include "Poseidon/Window.h"

#include <Hippios/Renderer.h>
#include <Hippios/GraphicsBuffer.h>
#include <Rhea/Core/Material.h>
#include <Rhea/Core/Shader.h>
#include <Rhea/Graphics/MaterialManager.h>
#include <Rhea/Core/Mesh.h>
#include <Rhea/Core/DirectionalLight.h>
#include <Rhea/Core/PointLight.h>
#include <Rhea/Core/SpotLight.h>
#include <Rhea/Animation/Skeleton.h>
#include <Rhea/Components/VisualComponent.h>
#include <Rhea/Components/RenderComponent.h>
#include <Psio/Log.h>
#include <algorithm>

namespace poseidon
{
	struct BoneBuffer {
		math::Matrix4 boneMatrices[100];
	};

	SceneManager::SceneManager() :
		m_renderer(nullptr),
		m_current_material(nullptr)
	{
		m_rend_queue = new RenderQueue();
	}

	SceneManager::~SceneManager()
	{
		// unique_ptrs handle cleanup of graphics buffers automatically
	}

	Node* SceneManager::get_root()
	{
		if (!m_tree) {
			m_tree = createNode();
		}
		return m_tree;
	}

	void SceneManager::setRenderer(hippios::Renderer* renderer)
	{
		m_renderer = renderer;

		//initialize graphic buffers
		m_frame_buffer = m_renderer->createGraphicsBuffer(sizeof(cbPerFrame));
		m_renderer->bindGPUBuffer(m_frame_buffer.get(), 0, hippios::ShaderStage::Vertex);
		m_object_buffer = m_renderer->createGraphicsBuffer(sizeof(cbPerObject));
		//m_renderer->bindGPUBuffer(m_object_buffer.get(), 1, hippios::ShaderStage::Vertex);
		m_light_buffer = m_renderer->createGraphicsBuffer(sizeof(cbLight));
		m_renderer->bindGPUBuffer(m_light_buffer.get(), 2, hippios::ShaderStage::Fragment);		
		m_bone_buffer = m_renderer->createGraphicsBuffer(sizeof(BoneBuffer));
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

			if (!viewport->getRenderTarget())
				continue;
			
			// set render target as active
			viewport->getRenderTarget()->setActive();
			// clear with background color
			viewport->getRenderTarget()->clearPass();

			// iterate over render contexts of this viewport
			for (const auto& rc_it : viewport->getRenderContextList()) {
				RenderContext* context = rc_it.get();
				viewport->getRenderTarget()->setViewport(context->getSize());

				// cull pass	
				m_rend_queue->clear();
				context->cullObjects(m_rend_queue);

				// render pass
				Camera* camera = context->get_camera();
				if (!camera)
					continue;

				// fill per frame buffer
				per_frame_buf = {};
				math::Matrix4 frameSrc[3];
				frameSrc[0] = camera->getViewMatrix();
				frameSrc[1] = camera->getProjectionMatrixRS();
				frameSrc[2] = frameSrc[1] * frameSrc[0];

				math::Matrix4 frameDst[3];
				m_renderer->prepareMatricesForShader(frameDst, frameSrc, 3);

				per_frame_buf.v = frameDst[0];
				per_frame_buf.p = frameDst[1];
				per_frame_buf.vp = frameDst[2];

				if (camera->get_position())
					per_frame_buf.cameraPos = camera->get_position()->get_worldspace();

				m_frame_buffer->update((void*)&per_frame_buf);
				m_renderer->bindGPUBuffer(m_frame_buffer.get(), 0, hippios::ShaderStage::Vertex);

				// fill lights buffer
				std::vector<rhea::Light*> lights;
				context->getLights(lights);
				light_buf = {};

				// global ambient light
				light_buf.ambient = context->getAmbient();

				// iterate lights and add them to the light buffer
				int idx = 0;
				for (std::vector<rhea::Light*>::iterator it = lights.begin(); it != lights.end(); ++it) {
					if (idx >= MAX_LIGHTS) {
						break;
					}
					rhea::Light* light = doCast<rhea::Light>(*it);
					if (light) {
						light_buf.lights[idx].specular = light->getSpecular();
						light_buf.lights[idx].diffuse = light->getDiffuse();
						light_buf.lights[idx].lightColor = light->getColor();

						rhea::DirectionalLight* dir_light = doCast<rhea::DirectionalLight>(*it);
						if (dir_light) {
							light_buf.lights[idx].Type = 0;
							light_buf.lights[idx].direction = dir_light->getDirection();
						}
						else {
							rhea::PointLight* point_light = doCast<rhea::PointLight>(*it);
							if (point_light) {
								light_buf.lights[idx].Type = 1;
								Vector3 pos = point_light->getTransform()->get_position()->get_worldspace();
								light_buf.lights[idx].position = math::Quaternion(pos.x, pos.y, pos.z, 1.0f);
								math::Vector3 attenuation = point_light->getAttenuation();
								light_buf.lights[idx].ConstantAttenuation = attenuation.x;
								light_buf.lights[idx].LinearAttenuation = attenuation.y;
								light_buf.lights[idx].QuadraticAttenuation = attenuation.z;
							}
							else
							{
								rhea::SpotLight* spot_light = doCast<rhea::SpotLight>(*it);
								if (spot_light) {
									light_buf.lights[idx].Type = 2;
									Vector3 pos = point_light->getTransform()->get_position()->get_worldspace();
									light_buf.lights[idx].position = math::Quaternion(pos.x, pos.y, pos.z, 1.0f);
									pos = math::Vector3(-pos.x, -pos.y, -pos.z);
									pos.normalize();
									light_buf.lights[idx].direction = pos;
									math::Vector3 attenuation = spot_light->getAttenuation();
									light_buf.lights[idx].ConstantAttenuation = attenuation.x;
									light_buf.lights[idx].LinearAttenuation = attenuation.y;
									light_buf.lights[idx].QuadraticAttenuation = attenuation.z;
									light_buf.lights[idx].ConeAngle = spot_light->getConeAngle();
								}
							}
						}
					}
					idx++;
				}
				light_buf.num_lights = (float)idx;
				m_light_buffer->update((void*)&light_buf);
				m_renderer->bindGPUBuffer(m_light_buffer.get(), 2, hippios::ShaderStage::Fragment);

				Item* renderable = m_rend_queue->first();
				for (Item* renderable = m_rend_queue->first(); renderable != NULL; renderable = m_rend_queue->next()) {

					auto* vis = renderable->getComponent<rhea::VisualComponent>();
					if (!vis || !vis->isVisible()) continue;

					// get render information
					rhea::client::RenderComponent* r_comp = renderable->getComponent<rhea::client::RenderComponent>();					
					if (!r_comp)
						continue;

					// fill per object buffer
					rhea::Transform* transform = renderable->getTransform();
					Position* position = nullptr;
					if (transform)
						position = transform->get_derived_position();
					// check if renderable has a position
					if (position) {// model world transform matrix (also called world matrix)
						math::Matrix4 objectSrc[2];

						math::Matrix3x4 model_wtm = position->getWorldTransform();
						objectSrc[0] = model_wtm;
						objectSrc[1] = objectSrc[0].inverse().transpose();

						math::Matrix4 objectDst[2];
						m_renderer->prepareMatricesForShader(objectDst, objectSrc, 2);

						per_obj_buf.world_matrix = objectDst[0];
						// used for lighting							
						per_obj_buf.inverse = objectDst[1];
						
						m_object_buffer->update((void*)&per_obj_buf);
						m_renderer->bindGPUBuffer(m_object_buffer.get(), 1, hippios::ShaderStage::Vertex);
					}
					
					// get render mesh
					// Ensure GPU mesh is available
					poseidon::Mesh* mesh = r_comp->gpuMesh;
					if (!mesh) {
						continue;
					}
					
					// skeleton handling if any
					rhea::Skeleton* skeleton = r_comp->skeleton;
					if (skeleton) {
						// Fill the bone buffer
						BoneBuffer boneBuffer{};
						for (int i = 0; i < 100; ++i)
						{
							boneBuffer.boneMatrices[i] = math::Matrix4::IDENTITY;
						}
						const auto& boneMatrices = skeleton->getBoneMatrices();
						size_t max_bones = std::min(boneMatrices.size(), static_cast<size_t>(100)); // Ensure we do not exceed buffer capacity.
						for (size_t i = 0; i < max_bones; ++i)
							boneBuffer.boneMatrices[i] = boneMatrices[i];

						// Update the bone matrix buffer
						m_bone_buffer->update((void*)&boneBuffer);
						m_renderer->bindGPUBuffer(m_bone_buffer.get(), 4, hippios::ShaderStage::Vertex); // Bind bone buffer to slot 4 for vertex shader
					}

					// iterate over submeshes
					for (size_t i = 0; i < mesh->getMeshCount(); ++i) {
						// access submesh i
						Mesh::SubMesh* submesh = mesh->getSubMesh(i);
						if (!submesh)
							continue;

						Material* mat = submesh->material;

						if (!mat) {
							DLOG(LOG_DEBUG) << "SceneManager::render: submesh " << i << " has no material - SKIPPING";
							continue;
						}

						if (mat != m_current_material && mat) {
							m_current_material = mat;
							mat->activate(3, 5);
						}
						if (!m_current_material) {
							printf("SceneManager::render: No shader found\n");
							return true;
						}					

						// set texture(s)

						std::vector<poseidon::Texture*> texs = m_current_material->getTextures();
						if (texs.size()) {
							for (int i = 0; i < texs.size(); i++) {
								m_renderer->setTexture(texs[i]->get_texture(), i);
							}
						}

						// draw the entity

						// bind the vertex and index buffer
						hippios::VertexBuffer* vb;
						hippios::IndexBuffer* ib;
						int num_indices;

						mesh->getRenderInput(i, &vb, &ib, num_indices);

						if (!vb) {
							DLOG(LOG_DEBUG) << "SceneManager::render: submesh " << i << " has no vertex buffer";
							continue;
						}

						if (!mesh->isIndexed()) {
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
							else {
								DLOG(LOG_DEBUG) << "SceneManager::render: indexed mesh but num_indices=0 or no vb";
							}
						}						
					}
				}
			}
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

	Window* SceneManager::getWindow(psio::Window* win)
	{
		for (std::vector<Window*>::iterator it = m_windows.begin(); it != m_windows.end(); ++it) {
			Window* window = *it;
			if (window->getNativeWindow() == win)
				return window;
		}
		return nullptr;
	}

}