#define NOMINMAX
#include "Poseidon/FrameRenderer.h"
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
#include "Poseidon/RenderWorld.h"
#include "Poseidon/Light.h"

#include <Hippios/Renderer.h>
#include <Hippios/GraphicsBuffer.h>
#include <Rhea/Core/Material.h>
#include <Rhea/Core/Shader.h>
#include <RheaClient/Graphics/MaterialManager.h>
#include <Rhea/Core/Mesh.h>
#include <Rhea/Animation/Skeleton.h>
#include <Rhea/Components/VisualComponent.h>
#include <RheaClient/Components/RenderComponent.h>
#include <Psio/Log.h>
#include <algorithm>

namespace poseidon
{
	struct BoneBuffer {
		math::Matrix4 boneMatrices[100];
	};

	FrameRenderer::FrameRenderer() :
		m_renderer(nullptr),
		m_current_material(nullptr)
	{
		m_rend_queue = new RenderQueue();
	}

	FrameRenderer::~FrameRenderer()
	{
		// unique_ptrs handle cleanup of graphics buffers automatically
	}

	Node* FrameRenderer::get_root()
	{
		if (!m_tree) {
			m_tree = createNode();
		}
		return m_tree;
	}

	void FrameRenderer::setRenderer(hippios::Renderer* renderer)
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

	Node* FrameRenderer::createNode()
	{
		Node* node = new Node();
		return node;
	}

	bool FrameRenderer::update()
	{
		return false;
	}

	bool FrameRenderer::render(RenderWorld* world)
	{
		for (Window* window : m_windows) {
			if (!window)
				continue;
			if (!renderWindow(window, world))
				return false;
		}
		return true;
	}

	bool FrameRenderer::renderWindow(Window* window, RenderWorld* world)
	{
		if (!window)
			return false;

		m_renderer->setActiveSwapChain(window->getSwapChain());

		hippios::RenderTarget* target = window->getRenderTarget();
		if (!target)
			return false;

		// set render target as active
		target->setActive();
		// clear with background color
		target->clearPass();

		// iterate over all viewports and execute the phases per viewport
		for (const auto& vp_ptr : window->getViewports()) {
			Viewport* viewport = vp_ptr.get();
			if (!viewport)
				continue;
			if (!renderViewport(viewport, target, world))
				return false;
		}
		return true;
	}

	bool FrameRenderer::renderViewport(Viewport* viewport, hippios::RenderTarget* target, RenderWorld* world)
	{
		if (!viewport)
			return false;

		hippios::ViewportDesc rect;
		rect.x = (float)viewport->getX();
		rect.y = (float)viewport->getY();
		rect.w = (float)viewport->getWidth();
		rect.h = (float)viewport->getHeight();
		rect.min_depth = 0.0f;
		rect.max_depth = 1.0f;
		target->setViewport(&rect);

		// iterate over render contexts of this viewport
		for (const auto& rc_it : viewport->getRenderContextList()) {
			RenderContext* context = rc_it.get();
			if (!context)
				continue;
			if (!renderContext(context, target, world))
				return false;
		}
		return true;
	}

	bool FrameRenderer::renderContext(RenderContext* context, hippios::RenderTarget* target, RenderWorld* world)
	{
		target->setViewport(context->getSize());

		// cull pass	
		m_rend_queue->clear();
		context->cullObjects(m_rend_queue);

		// render pass
		
		// resolve camera from context
		Camera* camera = nullptr;
		if (context->getCameraHandle() != InvalidCamera)
			camera = world->resolveCamera(context->getCameraHandle());

		if (!camera)
			return true; // not an error, just nothing to render for this context

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
		light_buf = {};
		light_buf.ambient = context->getAmbient();
		
		int idx = 0;
		for (poseidon::LightHandle handle : context->getLights())
		{
			if (idx >= MAX_LIGHTS)
				break;

			poseidon::Light* light = world->resolveLight(handle);
			if (!light)
				continue;

			if (!light->isVisible())
				continue;

			auto& dst = light_buf.lights[idx];
			dst.specular = light->getSpecular();
			dst.diffuse = light->getDiffuse();
			dst.lightColor = light->getColor();

			switch (light->getType())
			{
			case poseidon::Light::LightType::Directional:
				dst.Type = 0.0f;
				dst.direction = light->getDirection();
				break;

			case poseidon::Light::LightType::Point:
			{
				dst.Type = 1.0f;

				math::Vector3 pos = light->getPosition();
				dst.position = math::Quaternion(pos.x, pos.y, pos.z, 1.0f);

				math::Vector3 attenuation = light->getAttenuation();
				dst.ConstantAttenuation = attenuation.x;
				dst.LinearAttenuation = attenuation.y;
				dst.QuadraticAttenuation = attenuation.z;
				break;
			}

			case poseidon::Light::LightType::Spot:
			{
				dst.Type = 2.0f;

				math::Vector3 pos = light->getPosition();
				dst.position = math::Quaternion(pos.x, pos.y, pos.z, 1.0f);
				dst.direction = light->getDirection();

				math::Vector3 attenuation = light->getAttenuation();
				dst.ConstantAttenuation = attenuation.x;
				dst.LinearAttenuation = attenuation.y;
				dst.QuadraticAttenuation = attenuation.z;
				dst.ConeAngle = light->getConeAngle();
				break;
			}
			}

			++idx;
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
					DLOG(LOG_DEBUG) << "FrameRenderer::render: submesh " << i << " has no material - SKIPPING";
					continue;
				}

				if (mat != m_current_material && mat) {
					m_current_material = mat;
					mat->activate(3, 5);
				}
				if (!m_current_material) {
					printf("FrameRenderer::render: No shader found\n");
					return false;
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
					DLOG(LOG_DEBUG) << "FrameRenderer::render: submesh " << i << " has no vertex buffer";
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
						DLOG(LOG_DEBUG) << "FrameRenderer::render: indexed mesh but num_indices=0 or no vb";
					}
				}
			}
		}
		return true;
	}

	Window* FrameRenderer::getWindow(psio::Window* win)
	{
		for (std::vector<Window*>::iterator it = m_windows.begin(); it != m_windows.end(); ++it) {
			Window* window = *it;
			if (window->getNativeWindow() == win)
				return window;
		}
		return nullptr;
	}

}