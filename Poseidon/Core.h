#ifndef POSEIDON_CORE_H
#define POSEIDON_CORE_H

#include <Psio/Psio.h>

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>

#include "Hippios/Engine.h"
#include "Hippios/Window.h"
#include "Hippios/Renderer.h"

#include "Node.h"

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
		std::unique_ptr<Hippios::Engine> m_renderer;
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
		~Core();
		void init(Psio::Engine* psio);
		SceneManager* get_scene_manager() { return m_scene_manager; }
		// handle one frame
		int iterate();
		// loop with onFrameCallback
		int loop(std::function<void()> onFrame);

		// get psio engine
		Psio::Engine* getPsio() { return m_psio; }

		Hippios::Renderer* get_renderer() { return m_renderer->getRenderer(); }
		Hippios::Engine* get_engine() { return m_renderer.get(); }

		// create various items

		// create pipeline state object
		struct PipelineProperties {
			Shader* shader = nullptr;
			bool alpha_blending = false;
			bool depth_test = true;
			bool depth_write = true;
			bool cull_face = true;
			int cull_face_mode = 0;
			int topology = 4;			
			std::string stencil_test;
		};

		Hippios::PipelineStateObject* CreateOrGetPso(PipelineProperties& props);		

		// create Texture
		Texture* createTexture(int type, int num_images);

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

	protected:
		
		struct PipelinePropertiesHash {
			size_t operator()(const PipelineProperties& props) const {
				// Combine hashes of individual members
				size_t hash = std::hash<Shader*>{}(props.shader);
				hash_combine(hash, props.alpha_blending);
				hash_combine(hash, props.depth_test);
				hash_combine(hash, props.depth_write);
				hash_combine(hash, props.cull_face);
				hash_combine(hash, props.cull_face_mode);
				hash_combine(hash, props.topology);
				hash_combine(hash, props.stencil_test);
				return hash;
			}

		private:
			// Utility function to combine hashes
			static void hash_combine(size_t& seed, const bool& v) {
				seed ^= std::hash<bool>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
			}

			template <typename T>
			static void hash_combine(size_t& seed, const T& v) {
				seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
			}
		};

		struct PipelinePropertiesEqual {
			bool operator()(const PipelineProperties& lhs, const PipelineProperties& rhs) const {
				// Compare individual members for equality
				return lhs.shader == rhs.shader &&
					lhs.alpha_blending == rhs.alpha_blending &&
					lhs.depth_test == rhs.depth_test &&
					lhs.depth_write == rhs.depth_write &&
					lhs.cull_face == rhs.cull_face &&
					lhs.cull_face_mode == rhs.cull_face_mode &&
					lhs.topology == rhs.topology &&
					lhs.stencil_test == rhs.stencil_test;
			}
		};

		std::unordered_map<PipelineProperties, std::unique_ptr<Hippios::PipelineStateObject>, PipelinePropertiesHash, PipelinePropertiesEqual> m_pso_list;		
	};
}
#endif // !POSEIDON_CORE_H
