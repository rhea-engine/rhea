#include "Psio/Psio.h"
#include "Hippios/Window.h"
#include "Hippios/Viewport.h"

#include "Texture.h"
#include "Viewport.h"
#include "Material.h"
#include "Entity.h"
#include "Scene.h"

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

		// add a root viewport
		m_viewport = new Viewport();		
		m_viewport->initRenderTarget(m_renderer);

		// add a root scene
		m_scene = new Scene();
	}

	int Core::loop()
	{
		while (!quit)
		{
			// check for window events
			if (Psio::Engine::tick())
				quit = true;

			// update passes

			if (m_scene->update())
				quit = true;

			// main pass: render frame
			//if (m_renderer->render())
			// quit = true;
			
		}
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
		return entity;
	}

	Camera* Core::createCamera()
	{
		Camera* camera = new Poseidon::Camera(m_renderer->getRenderer());
		return camera;
	}
}