#ifndef POSEIDON_VIEWPORT_H
#define POSEIDON_VIEWPORT_H

#include <Hippios/Renderer.h>

#include "Camera.h"
#include "Node.h"
#include <Hippios/RenderTarget.h>
#include <memory>

namespace poseidon
{
	class Renderable;
	class Entity;
	class RenderQueue;
	class Camera;
	class RenderContext;

	/// <summary>
	/// Viewport is one render target, where the rendering occurs (backbuffers etc.)
	/// Not to confuse with a renderer viewport which is simply a rectangle of the the render target
	/// The Viewport has a list of render contexts.
	/// </summary>
	class Viewport : public Node
	{
	public: 		
		Viewport() :
			Viewport(std::string("root"))
		{}
		Viewport(std::string name);
		~Viewport();

		virtual void initRenderTarget(hippios::Renderer* renderer);
		hippios::Renderer* get_renderer() { return m_renderer; };

		// render contexts
		RenderContext* getRenderContext(std::string& name);
		RenderContext* createRenderContext(std::string& name);
		std::vector<std::unique_ptr<RenderContext>>& getRenderContextList() { return m_render_contexts; }

		// name handling
		void setName(std::string& name) { m_name = name; }
		std::string getName() { return m_name; }
		bool isName(std::string& name) { return m_name == name; }

		// resize the viewport
		virtual void onResize(int width, int height);

		// create my render target
		// destroys old render target and all render contexts
		// creates a default render context with the size of the target
		void createRenderTarget(int width, int height, hippios::SwapChain* swap_chain);
		// get the render target
		hippios::RenderTarget* getRenderTarget() { return m_render_target.get(); }

		// get width of viewport
		int getWidth() { return m_width; }
		// get height of viewport
		int getHeight() { return m_height; }

		// get the swapchain
		hippios::SwapChain* getSwapChain() { return m_swapchain; }

	protected:
		std::string m_name;
		Camera* m_camera = 0; // the active camera
		hippios::Renderer* m_renderer = 0; // reference to the render system
		hippios::SwapChain* m_swapchain;

		int m_width;
		int m_height;

		std::vector<std::unique_ptr<RenderContext>> m_render_contexts;
		std::unique_ptr<hippios::RenderTarget> m_render_target;

		virtual void registerNode();
		virtual void registerGroup(std::string group_name, Node* node);
	};

}

#endif // !POSEIDON_VIEWPORT_H