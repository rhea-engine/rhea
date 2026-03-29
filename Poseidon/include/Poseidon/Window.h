#ifndef POSEIDON_WINDOW_H
#define POSEIDON_WINDOW_H

#include "Viewport.h"
#include <Psio/Psio.h>
#include <Psio/Window.h>
#include <Hippios/SwapChain.h>
#include <Hippios/Renderer.h>
#include <memory>

namespace Poseidon
{
	class Window : public Viewport
	{
	public:
		Window(std::string& name) :
			Viewport(name)
		{}
		void initRenderTarget(Hippios::Renderer* renderer);
		void CreateFromNativeWindow(Psio::Engine* psio, std::string name, int width, int height);
		void onResize(int width, int height) override;

		// set fullscreen mode
		void SetFullscreen(bool mode);

		// get the renderer swpachain of this window
		Hippios::SwapChain* getSwapChain() { return m_swap_chain.get(); }

		Psio::Window* getNativeWindow() { return m_native_win.get(); }
	protected:
		std::unique_ptr<Hippios::SwapChain> m_swap_chain;
		std::unique_ptr<Psio::Window> m_native_win;
		std::string m_win_name;
	};
}
#endif // !POSEIDON_WINDOW_H
