#include "Poseidon/Window.h"
#include <memory>
#include <Hippios/Renderer.h>
#include <Psio/Log.h>

namespace Poseidon
{
	void Window::initRenderTarget(Hippios::Renderer* renderer)
	{
		Viewport::initRenderTarget(renderer);
	}

	void Window::CreateFromNativeWindow(Psio::Engine* psio, std::string name, int width, int height)
	{
		Psio::WindowOption options = Psio::WindowOption::None;
		// if renderer is OpenGL, set the option for creating an OpenGL context
		if (m_renderer) {
			if (m_renderer->getRendererBackend() == Hippios::RendererBackend::OpenGL)
				options |= Psio::WindowOption::OpenGL;
		}

		m_native_win = psio->createWindow(name, width, height, options);
		m_win_name = name;
		m_render_window.width = m_native_win->GetWidth();
		m_render_window.height = m_native_win->GetHeight();
		m_render_window.native_win_handle = m_native_win->getHandle();

		m_swap_chain = m_renderer->createSwapChain(&m_render_window);
		if (!m_swap_chain) {
			LOG(LOG_ERROR) << "Failed to create swap chain and initialize graphics context";
			return; // or throw exception
		}
		
		// create my own render target
		createRenderTarget(m_render_window.width, m_render_window.height, m_swap_chain.get());
	}

	void Window::onResize(int width, int height)
	{
		if (m_width == width && m_height == height)
			return;

		if (!m_render_target || !m_swap_chain) {
			LOG(LOG_ERROR) << "Window::onResize: no render target or swap chain";
			return;
		}
		m_render_target->Release();
		m_swap_chain->onResize(width, height);
		Viewport::onResize(width, height);
	}

	void Window::SetFullscreen(bool mode)
	{
		bool exclusive_fullscreen = false;
		if (exclusive_fullscreen)
			m_swap_chain->SetFullscreen(mode);
		else
			m_native_win->SetFullscreen(mode);
	}
}
