#include "Psio/Psio.h"
#include "Hippios/Window.h"
#include "Hippios/Viewport.h"

#include "Texture.h"
#include "Material.h"
#include "Entity.h"
#include "SceneManager.h"
#include "Window.h"

#include "Core.h"


namespace Poseidon
{
	Core::Core()
	{		
	}

	void Core::init()
	{
		// initialize render sub system
		m_renderer = new Hippios::Engine();
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

	int Core::loop()
	{
		while (!quit)
		{
			// check for window events
			if (Psio::Engine::tick())
				quit = true;

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
		// check for window events
		if (Psio::Engine::tick())
			quit = true;

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

	Texture* Core::createTexture(const std::string& file_name)
	{
		Poseidon::Texture* texture = new Poseidon::Texture();
		texture->bind(m_renderer);
		if (texture->load(file_name))
			return texture;
		else {
			delete texture;
			return NULL;
		}
	}

	Material* Core::createMaterial()
	{
		return new Material(this->m_renderer->getRenderer());
	}

	Entity* Core::createEntity(Mesh* mesh)
	{
		Entity* entity;
		entity = new Entity();
		entity->set_mesh(mesh);
		entity->createPosition(0);
		return entity;
	}

	Camera* Core::createCamera()
	{
		Camera* camera = new Poseidon::Camera(m_renderer->getRenderer());
		return camera;
	}
}