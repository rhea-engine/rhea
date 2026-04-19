#ifndef POSEIDON_WINDOW_H
#define POSEIDON_WINDOW_H

#include "Viewport.h"
#include <Psio/Psio.h>
#include <Psio/Window/Window.h>
#include <Hippios/SwapChain.h>
#include <Hippios/Renderer.h>
#include <memory>

namespace poseidon
{
	class Window
	{
	public:
		Window(const std::string& name) :
			m_name(name)
		{}
		~Window();

		void init(hippios::Renderer* renderer,
			psio::Engine* psio,
			const std::string& title,
			int width,
			int height);

		// on resize event, resize the swapchain and render target
		void onResize(int width, int height);
		// set fullscreen mode
		void SetFullscreen(bool mode);

		const std::string& getName() const { return m_name; }

		hippios::Renderer* getRenderer() const { return m_renderer; }

		// get the renderer swpachain of this window
		hippios::SwapChain* getSwapChain() { return m_swap_chain.get(); }

		// create my render target
		// destroys old render target and all render contexts
		// creates a default render context with the size of the target
		void createRenderTarget(int width, int height, hippios::SwapChain* swap_chain);
		// get the render target
		hippios::RenderTarget* getRenderTarget() { return m_render_target.get(); }

		psio::Window* getNativeWindow() { return m_native_win.get(); }

		Viewport* createViewport(const std::string& name, int x, int y, int width, int height);
		void addViewport(std::unique_ptr<Viewport> viewport);
		void removeViewport(Viewport* viewport);
		Viewport* getViewport(const std::string& name);
		Viewport* getDefaultViewport() { return m_viewports.empty() ? nullptr : m_viewports[0].get(); }

		const std::vector<std::unique_ptr<Viewport>>& getViewports() const { return m_viewports; }

	protected:
		std::string m_name;
		hippios::Renderer* m_renderer = nullptr;

		std::unique_ptr<hippios::SwapChain> m_swap_chain;
		std::unique_ptr<psio::Window> m_native_win;
		std::unique_ptr<hippios::RenderTarget> m_render_target;
		hippios::NativeWindow m_render_window{};

		std::string m_win_name;
		int m_width = 0;
		int m_height = 0;

		std::vector<std::unique_ptr<Viewport>> m_viewports;
	};
}
#endif // !POSEIDON_WINDOW_H
