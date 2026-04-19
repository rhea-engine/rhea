#include "Psio/Psio.h"

#include "Poseidon/Texture.h"
#include "Poseidon/Material.h"
#include "Poseidon/Shader.h"
#include "Poseidon/Entity.h"
#include "Poseidon/SceneManager.h"
#include "Poseidon/Window.h"
#include "Poseidon/Cubemap.h"

#include "Poseidon/Core.h"
#include <Psio/Log.h>

#include <Hippios/Renderer.h>
#include <Hippios/SwapChain.h>
using hippios::SamplerDesc;

namespace poseidon
{
	Core::Core()
	{
		m_psio = nullptr;
		m_appname = "Poseidon";
	}

	Core::~Core()
	{
		// delete all shaders
		for (auto it = m_shaders.begin(); it != m_shaders.end(); ++it)
		{
			delete it->second;
		}
		m_shaders.clear();
	}

	void Core::init(psio::Engine* psio, std::string backend)
	{
		m_psio = psio;

		// 1. Determine backends
		hippios::WindowBackend win_backend = determineWindowBackend(m_psio->getBackendName());
		hippios::RendererBackend renderer_backend = determineRendererBackend(backend);
		
		// 2. Create renderer (independent of windows)
		m_renderer = hippios::Renderer::CreateRenderer(renderer_backend, win_backend);

		// 3. Create default render states (renderer-wide, not per-window)
		createDefaultStencilStates();
		createDefaultSamplers();

		// 4. Create scene manager
		m_scene_manager = new SceneManager();
		m_scene_manager->setRenderer(m_renderer.get());		
	}

	Window* Core::createWindow(const std::string& title, int width, int height, const std::string& name)
	{
		auto* window = new Window(name);
		window->init(m_renderer.get(), m_psio, title, width, height);

		m_windows.push_back(window);

		if (m_scene_manager)
			m_scene_manager->registerWindow(window);

		return window;
	}

	void Core::createDefaultStencilStates()
	{
		hippios::DepthStencilStateDesc desc;
		
		// Default state
		desc.depth_enable = true;
		desc.depth_write_mask = hippios::DepthWriteMask::DEPTH_WRITE_ALL;
		desc.depth_func = hippios::ComparisonFunction::COMPARE_LESS;
		desc.stencil_enable = false;
		m_renderer->createStencilState("default", desc);

		// Disabled state
		desc.depth_enable = false;
		desc.depth_write_mask = hippios::DepthWriteMask::DEPTH_WRITE_ZERO;
		desc.depth_func = hippios::ComparisonFunction::COMPARE_ALWAYS;
		desc.stencil_enable = false;
		m_renderer->createStencilState("disabled", desc);
		
		// Skybox state
		desc.depth_enable = true;
		desc.depth_write_enable = false;
		desc.depth_write_mask = hippios::DepthWriteMask::DEPTH_WRITE_ZERO;
		desc.depth_func = hippios::ComparisonFunction::COMPARE_LESS_EQUAL;
		desc.stencil_enable = false;
		m_renderer->createStencilState("skybox", desc);
	}

	void Core::createDefaultSamplers()
	{
		hippios::SamplerDesc sampler_desc;
		
		// Linear sampler
		memset(&sampler_desc, 0, sizeof(sampler_desc));
		sampler_desc.filter = hippios::SamplerDesc::Filter::LINEAR;
		sampler_desc.address_u = hippios::SamplerDesc::AddressMode::WRAP;
		sampler_desc.address_v = hippios::SamplerDesc::AddressMode::WRAP;
		sampler_desc.address_w = hippios::SamplerDesc::AddressMode::WRAP;
		sampler_desc.comparison_func = hippios::ComparisonFunction::COMPARE_NEVER;
		sampler_desc.min_lod = 0;
		sampler_desc.max_lod = FLT_MAX;
		
		hippios::SamplerState* linear_sampler = m_renderer->createSampler(sampler_desc);
		if (linear_sampler)
			m_sampler_states["linear"] = linear_sampler;
		else
			LOG(LOG_ERROR) << "Could not create linear sampler";
		
		// Nearest sampler
		memset(&sampler_desc, 0, sizeof(sampler_desc));
		sampler_desc.filter = hippios::SamplerDesc::Filter::MIP_POINT;
		sampler_desc.address_u = hippios::SamplerDesc::AddressMode::CLAMP;
		sampler_desc.address_v = hippios::SamplerDesc::AddressMode::CLAMP;
		sampler_desc.address_w = hippios::SamplerDesc::AddressMode::CLAMP;
		sampler_desc.comparison_func = hippios::ComparisonFunction::COMPARE_NEVER;
		sampler_desc.min_lod = 0;
		sampler_desc.max_lod = FLT_MAX;
		
		hippios::SamplerState* nearest_sampler = m_renderer->createSampler(sampler_desc);
		if (nearest_sampler)
			m_sampler_states["nearest"] = nearest_sampler;
		else
			LOG(LOG_ERROR) << "Could not create nearest sampler";
	}

	hippios::WindowBackend Core::determineWindowBackend(const std::string& backend_name)
	{
		if (backend_name == "Win32") return hippios::WindowBackend::Win32;
		if (backend_name == "X11") return hippios::WindowBackend::X11;
		if (backend_name == "Wayland") return hippios::WindowBackend::Wayland;
		if (backend_name == "SDL") return hippios::WindowBackend::SDL;
		if (backend_name == "GLFW") return hippios::WindowBackend::GLFW;
		
		LOG(LOG_INFO) << "Unknown window backend: " << backend_name << ", defaulting to GLFW";
		return hippios::WindowBackend::GLFW;
	}

	hippios::RendererBackend Core::determineRendererBackend(const std::string& backend)
	{
		if (backend == "d3d11") {
#if defined(_WIN32) || defined(_WIN64)
			m_renderer_backend = "d3d11";
			return hippios::RendererBackend::D3D11;
#else
			LOG(LOG_INFO) << "D3D11 not supported on this platform, using OpenGL";
			m_renderer_backend = "opengl";
			return hippios::RendererBackend::OpenGL;
#endif
		}
		
		if (backend == "opengl") {
			m_renderer_backend = "opengl";
			return hippios::RendererBackend::OpenGL;
		}
		
		// Auto-detect
#if defined(_WIN32) || defined(_WIN64)
		m_renderer_backend = "d3d11";
		return hippios::RendererBackend::D3D11;
#else
		m_renderer_backend = "opengl";
		return hippios::RendererBackend::OpenGL;
#endif
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

			// present pass for all windows
			for (auto& win : m_windows) {
				if (m_renderer->presentPass(win->getSwapChain()))
					quit = true;
			}
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

		if (!quit) {
			// present pass for all windows
			for (auto& win : m_windows) {
				// prepare swapchain for rendering
				if (!m_renderer->setActiveSwapChain(win->getSwapChain()))
					return 1;

				
				if (m_renderer->presentPass(win->getSwapChain()))
					quit = true;
			}
		}

		if (quit)
			return 1;
		else
			return 0;
	}

	Texture* Core::createTexture(int type, int num_images)
	{
		poseidon::Texture* texture;
		if (type==0)
			texture = new poseidon::Texture();
		else {
			texture = new poseidon::Cubemap();
			texture->setNumImages(num_images);
		}
		texture->bind(m_renderer.get());
		return texture;
	}

	Material* Core::createMaterial()
	{
		return new Material(this);
	}

	void Core::registerMaterial(std::string name, Material* material)
	{
		m_materials[name] = material;

		// the first material or the one with the name "default" becomes the default 
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
		Shader* shader = new Shader(this);
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
		Camera* camera = new poseidon::Camera(m_renderer.get());
		return camera;
	}

	hippios::SamplerState* Core::getSamplerState(std::string name)
	{
		if (m_sampler_states.find(name) != m_sampler_states.end())
			return m_sampler_states[name];
		else
			return nullptr;
	}

	void Core::SetFullscreen(bool mode)
	{
		m_windows[0]->SetFullscreen(mode);
	}

	hippios::GraphicsBuffer* Core::getGraphicsBuffer(std::string facility, size_t size)
	{
		BufferDescription desc = { facility, size };

		// check if buffer exists already
		auto it = m_buffer_pool.find(desc);
		if (it != m_buffer_pool.end())
			return it->second.get();

		// no such buffer in the pool, create a new one
		auto newBuffer = m_renderer->createGraphicsBuffer(size);
		auto* raw = newBuffer.get();
		m_buffer_pool[desc] = std::move(newBuffer);
		return raw;
	}
}