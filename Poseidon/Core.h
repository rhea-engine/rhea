#ifndef POSEIDON_CORE_H
#define POSEIDON_CORE_H

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>

#include "Hippios/Engine.h"
#include "Hippios/Window.h"
#include "Hippios/Renderer.h"

#include "Node.h"

namespace Psio
{
	class Engine;
}

namespace Poseidon
{
	class Material;
	class Shader;
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

		Psio::Engine* m_psio;
		std::unordered_map<std::string, Material*> m_materials;
		std::unordered_map<std::string, Shader*> m_shaders;

		Material* m_default_material = 0;
	public:
		Core();
		void init(Psio::Engine* psio);
		SceneManager* get_scene_manager() { return m_scene_manager; }
		// handle one frame
		int iterate();
		// loop with onFrameCallback
		int loop(std::function<void()> onFrame);

		// get psio engine
		Psio::Engine* getPsio() { return m_psio; }

		Hippios::Renderer* get_renderer() { return m_renderer->getRenderer(); };

		// create various items
		Texture* createTexture(const std::string& file_name);
		Texture* createTexture(unsigned char* buf, size_t buflen);
		// materials
		Material* createMaterial();
		void registerMaterial(std::string name, Material* material);
		Material* getMaterial(std::string name);
		Material* getDefaultMaterial() { return m_default_material; }
		// shaders
		// get the shader with this name 
		Shader* getShader(std::string name);
		// create a new shader and add it with name
		Shader* createShader(std::string name);

		Entity* createEntity(Mesh* mesh);
		Camera* createCamera();
	};
}
#endif // !POSEIDON_CORE_H
