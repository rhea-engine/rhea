#ifndef POSEIDON_CORE_H
#define POSEIDON_CORE_H

#include <Psio/Psio.h>
#include <Hippios/Renderer.h>

#include <string>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

namespace hippios { class Renderer; class GraphicsBuffer; class SamplerState; }

#include "Node.h"

namespace poseidon
{
	class Material;
	class Shader;
	class Texture;
	class Entity;
	class Mesh;
	class Window;
	class FrameRenderer;
	class Camera;
	class RenderWorld;

	/** The holder of the scenen tree
		Acts as a gateway to the grahpics engine system
	*/
	class Core
	{	
	public:
		Core();
		~Core();
		void init(psio::Engine* psio, std::string backend);

		// handle one frame
		int iterate();
		// loop with onFrameCallback
		int loop(std::function<void()> onFrame);

		// get psio engine
		psio::Engine* getPsio() { return m_psio; }

		hippios::Renderer* get_renderer() { return m_renderer.get(); }
		std::string getRendererBackend() { return m_renderer_backend; }

		// create various items

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

		// get sampler state by name
		hippios::SamplerState* getSamplerState(std::string name);

		// set application name
		void setAppname(std::string appname) { m_appname = appname; }

		// fullscreen mode
		void SetFullscreen(bool mode);

		hippios::GraphicsBuffer* getGraphicsBuffer(std::string facility, size_t size);		

		// Create additional windows (uses same renderer & states)
		Window* createWindow(const std::string& title, int width, int height, const std::string& name = "");

		void initializeRenderStates();

		RenderWorld* getRenderWorld() { return m_render_world.get(); }
		FrameRenderer* getFrameRenderer() { return m_frame_renderer.get(); }
	
	private:
		std::unique_ptr<RenderWorld> m_render_world;
		std::unique_ptr<FrameRenderer> m_frame_renderer;

		void createDefaultStencilStates();
		void createDefaultSamplers();
		hippios::WindowBackend determineWindowBackend(const std::string& backend_name);
		hippios::RendererBackend determineRendererBackend(const std::string& backend);
	
		std::unique_ptr<hippios::Renderer> m_renderer;
		int quit = 0;						
		std::string m_appname;
		std::string m_renderer_backend;		

		psio::Engine* m_psio;
		std::unordered_map<std::string, Material*> m_materials;
		std::unordered_map<std::string, Shader*> m_shaders;
		std::unordered_map<std::string, hippios::SamplerState*> m_sampler_states;

		Material* m_default_material = 0;

		// graphics buffer pool
		struct BufferDescription
		{
			std::string facility;
			size_t size;
		};
		// Hash function for Element
		struct BufferDescriptionHash {
			std::size_t operator()(const BufferDescription& e) const {
				// Combine hashes of name and size
				std::hash<std::string> stringHash;
				std::hash<size_t> sizeHash;
				return stringHash(e.facility) ^ (sizeHash(e.size) << 1);
			}
		};

		// Equality function for BufferDescription
		struct BufferDescriptionEqual {
			bool operator()(const BufferDescription& lhs, const BufferDescription& rhs) const {
				// Compare both name and size
				return lhs.facility == rhs.facility && lhs.size == rhs.size;
			}
		};
		std::unordered_map<BufferDescription, std::unique_ptr<hippios::GraphicsBuffer>, BufferDescriptionHash, BufferDescriptionEqual> m_buffer_pool;
		std::vector<Window*> m_windows; // For multi-window support
	};
}
#endif // !POSEIDON_CORE_H
