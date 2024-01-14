#include "Psio/Psio.h"
#include "Hippios/Window.h"
#include "Hippios/Viewport.h"

#include "Texture.h"
#include "Material.h"
#include "Shader.h"
#include "Entity.h"
#include "SceneManager.h"
#include "Window.h"
#include "Cubemap.h"

#include "Core.h"


namespace Poseidon
{
	Core::Core()
	{
		m_psio = nullptr;	
	}

	void Core::init(Psio::Engine* psio)
	{
		// initialize the psio subsystem
		m_psio = psio;

		// initialize render sub system
		m_renderer = new Hippios::Engine(m_psio);
		m_renderer->startup();		

		// add a root window which is also the root viewport
		m_window = new Window();		
		m_window->initRenderTarget(m_renderer);

		// add a scene manager 
		m_scene_manager = new SceneManager();
		m_scene_manager->setRenderer(m_renderer->getRenderer());
		m_scene_manager->setRoot(m_window);
		m_scene_manager->registerViewport(m_window);
	}

	int Core::loop(std::function<void()> onFrame)
	{
		while (!quit)
		{
			//  callback for onFrame event
			onFrame();

			// update passes

			if (m_scene_manager->update())
				quit = true;

			// main pass: render frame
			if (m_scene_manager->render())
				quit = true;
		}
		return 0;
	}

	int Core::iterate()
	{
		// update passes

		if (m_scene_manager->update())
			quit = true;

		// main pass: render frame
		if (m_scene_manager->render())
			quit = true;

		if (quit)
			return 1;
		else
			return 0;
	}

	Hippios::PipelineStateObject* Core::CreateOrGetPso(PipelineProperties& props)
	{
		// Check if a PSO already exists for the given material
		auto it = m_pso_list.find(props);

		if (it == m_pso_list.end()) {
			// Create a new PSO
			Hippios::PipelineStateObjectDesc pso_desc;	

			// stencil state
			pso_desc.depthStencilState.name = props.stencil_test;

			// blend state
			if (props.alpha_blending) {
				pso_desc.blendState.renderTarget[0].blend_enable = true;
				pso_desc.blendState.renderTarget[0].src_blend = Hippios::BlendFactor::BLEND_ONE;
				pso_desc.blendState.renderTarget[0].dest_blend = Hippios::BlendFactor::BLEND_INV_SRC_ALPHA;
				pso_desc.blendState.renderTarget[0].blend_op = Hippios::BlendOp::BLEND_ADD;
				pso_desc.blendState.renderTarget[0].src_blend_alpha = Hippios::BlendFactor::BLEND_ONE;
				pso_desc.blendState.renderTarget[0].dest_blend_alpha = Hippios::BlendFactor::BLEND_ZERO;
				pso_desc.blendState.renderTarget[0].blend_op_alpha = Hippios::BlendOp::BLEND_ADD;
				pso_desc.blendState.renderTarget[0].render_target_write_mask = 0x0f;
			}
			else {
				pso_desc.blendState.renderTarget[0].blend_enable = false;
				pso_desc.blendState.renderTarget[0].src_blend = Hippios::BlendFactor::BLEND_ONE;
				pso_desc.blendState.renderTarget[0].dest_blend = Hippios::BlendFactor::BLEND_INV_SRC_ALPHA;
				pso_desc.blendState.renderTarget[0].blend_op = Hippios::BlendOp::BLEND_ADD;
				pso_desc.blendState.renderTarget[0].src_blend_alpha = Hippios::BlendFactor::BLEND_ONE;
				pso_desc.blendState.renderTarget[0].dest_blend_alpha = Hippios::BlendFactor::BLEND_ZERO;
				pso_desc.blendState.renderTarget[0].blend_op_alpha = Hippios::BlendOp::BLEND_ADD;
				pso_desc.blendState.renderTarget[0].render_target_write_mask = 0x0f;
			}

			// shader programs
			pso_desc.shaderProgram.vertex_shader = props.shader->getVShader();
			pso_desc.shaderProgram.pixel_shader = props.shader->getPShader();

			// layout
			pso_desc.vertexLayout.topology = props.topology;

			Hippios::PipelineStateObject* pso = m_renderer->getRenderer()->createPipelineState(&pso_desc);
			m_pso_list[props].reset(pso);
		}
		return m_pso_list[props].get();
	}

	Texture* Core::createTexture(int type, int num_images)
	{
		Poseidon::Texture* texture;
		if (type==0)
			texture = new Poseidon::Texture();
		else {
			texture = new Poseidon::Cubemap();
			texture->setNumImages(num_images);
		}
		texture->bind(m_renderer);
		return texture;
	}

	Material* Core::createMaterial()
	{
		return new Material(this);
	}

	void Core::registerMaterial(std::string name, Material* material)
	{
		m_materials[name] = material;

		// the first material or the one with the name default becomes the default 
		if (!m_default_material || (name == "default"))
			m_default_material = material;		
	}

	Material* Core::getMaterial(std::string name)
	{
		auto it = m_materials.find(name);
		if (it == m_materials.end())
			return nullptr;
		return it->second;
	}

	Shader* Core::getShader(std::string name)
	{	
		auto it = m_shaders.find(name);
		if (it != m_shaders.end())
			return it->second;
		else
			return nullptr;
	}

	Shader* Core::createShader(std::string name)
	{
		Shader* shader = new Shader(this->m_renderer->getRenderer());
		m_shaders[name] = shader;
		return shader;
	}

	Entity* Core::createEntity(Mesh* mesh)
	{
		Entity* entity;
		entity = new Entity();
		if (mesh)
			entity->set_mesh(mesh);
		entity->setMaterial(m_default_material);
		return entity;
	}

	Camera* Core::createCamera()
	{
		Camera* camera = new Poseidon::Camera(m_renderer->getRenderer());
		return camera;
	}
}