#ifndef POSEIDON_CORE_H
#define POSEIDON_CORE_H

#include <string>

#include "Hippios/Engine.h"
#include "Hippios/Window.h"
#include "Hippios/Renderer.h"

#include "Node.h"

namespace Poseidon
{
	class Material;
	class Texture;
	class Entity;
	class Mesh;
	class Window;
	class SceneManager;
	class Camera;

	/** The holder of the scenen tree
		Acts as a gateway to the grahpics engine system
	*/
	class Core
	{
	protected:
		Hippios::Engine* m_renderer = 0;
		int quit = 0;	
		Hippios::HippiosWindow* m_win = 0;
		Window* m_window = 0;
		SceneManager* m_scene_manager = 0;
	public:
		Core();
		void createWindow();
		void init();
		SceneManager* get_scene_manager() { return m_scene_manager; }
		int loop();
		int iterate();
		Hippios::Renderer* get_renderer() { return m_renderer->getRenderer(); };

		Texture* createTexture(const std::string& file_name);		
		Material* createMaterial();
		Entity* createEntity(Mesh* mesh);
		Camera* createCamera();
	};
}
#endif // !POSEIDON_CORE_H
