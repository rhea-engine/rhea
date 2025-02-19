#include "Window.h"
#include <memory>
#include <Hippios/Renderer.h>

namespace Poseidon
{
	void Window::initRenderTarget(Hippios::Renderer* renderer)
	{
		Viewport::initRenderTarget(renderer);
	}

	void Window::CreateFromNativeWindow(Psio::Engine* psio, int width, int height)
	{
		m_native_win = psio->createWindow(width, height);
		m_native_win->getHandle();
		Hippios::NativeWindow win;
		win.width = m_native_win->GetWidth();
		win.height = m_native_win->GetHeight();
		win.native_win_handle = m_native_win->getHandle();

		m_swap_chain.reset(m_renderer->createSwapChain(&win));
		// create my own render target
		createRenderTarget(win.width, win.height, m_swap_chain.get());
	}

	void Window::onResize(int width, int height)
	{
		if (m_width == width && m_height == height)
			return;

		m_render_target->Release();
		m_swap_chain->onResize(width, height);
		Viewport::onResize(width, height);
	}
}
