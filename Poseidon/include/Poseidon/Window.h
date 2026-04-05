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
	class Window : public Viewport
	{
	public:
		Window(std::string& name) :
			Viewport(name)
		{}
		void initRenderTarget(hippios::Renderer* renderer);
		void CreateFromNativeWindow(psio::Engine* psio, std::string name, int width, int height);
		void onResize(int width, int height) override;

		// set fullscreen mode
		void SetFullscreen(bool mode);

		// get the renderer swpachain of this window
		hippios::SwapChain* getSwapChain() { return m_swap_chain.get(); }

		psio::Window* getNativeWindow() { return m_native_win.get(); }
	protected:
		std::unique_ptr<hippios::SwapChain> m_swap_chain;
		std::unique_ptr<psio::Window> m_native_win;
		std::string m_win_name;
		hippios::NativeWindow m_render_window{};
	};
}
#endif // !POSEIDON_WINDOW_H
