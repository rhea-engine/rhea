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

	Core::~Core()
	{
		// delete all shaders
		for (auto it = m_shaders.begin(); it != m_shaders.end(); ++it)
		{
			delete it->second;
		}
		m_shaders.clear();
	}

	void Core::init(Psio::Engine* psio)
	{
		// initialize the psio subsystem
		m_psio = psio;

		// initialize render sub system
		m_renderer = std::make_unique<Hippios::Engine>(m_psio);
		m_renderer->startup();		

		// add a root window which is also the root viewport
		m_window = new Window();		
		m_window->initRenderTarget(m_renderer.get());

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

	Texture* Core::createTexture(int type, int num_images)
	{
		Poseidon::Texture* texture;
		if (type==0)
			texture = new Poseidon::Texture();
		else {
			texture = new Poseidon::Cubemap();
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
		Camera* camera = new Poseidon::Camera(m_renderer->getRenderer());
		return camera;
	}

	Hippios::GraphicsBuffer* Core::getGraphicsBuffer(std::string facility, size_t size)
	{
		BufferDescription desc = { facility, size };

		// check if buffer exists already
		auto it = m_buffer_pool.find(desc);
		if (it != m_buffer_pool.end())
			return it->second;

		// no such buffer in the pool, create a new one
		Hippios::GraphicsBuffer* newBuffer = m_renderer->getRenderer()->createGraphicsBuffer(size);
		m_buffer_pool[desc] = newBuffer;
		return newBuffer;
	}
}