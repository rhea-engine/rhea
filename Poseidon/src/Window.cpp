#include "Poseidon/Window.h"
#include <memory>
#include <Hippios/Renderer.h>
#include <Psio/Log.h>

namespace poseidon
{
	Window::~Window() = default;

	void Window::init(hippios::Renderer* renderer,
		psio::Engine* psio,
		const std::string& title,
		int width,
		int height)
	{
		m_renderer = renderer;

		psio::WindowDesc desc{};
		desc.name = title;
		desc.width = width;
		desc.height = height;

		if (m_renderer && m_renderer->getRendererBackend() == hippios::RendererBackend::OpenGL) {
			desc.backend.opengl = true;
			desc.backend.major_version = 3;
			desc.backend.minor_version = 3;
			desc.backend.core_profile = true;
#ifdef _DEBUG
			desc.backend.debug_context = true;
#endif
		}

		m_native_win = psio->createWindow(desc);
		if (!m_native_win) {
			LOG(LOG_ERROR) << "Failed to create native window";
			return;
		}

		m_width = m_native_win->GetWidth();
		m_height = m_native_win->GetHeight();

		m_render_window.width = m_width;
		m_render_window.height = m_height;
		m_render_window.native_win_handle = m_native_win->getHandle();

		m_swap_chain = m_renderer->createSwapChain(&m_render_window);
		if (!m_swap_chain) {
			LOG(LOG_ERROR) << "Failed to create swap chain";
			return;
		}

		hippios::RenderTargetDesc rt_desc{};
		rt_desc.type = hippios::RenderTargetType::SwapChain;
		rt_desc.width = m_width;
		rt_desc.height = m_height;
		rt_desc.swap_chain = m_swap_chain.get();
		m_render_target = m_renderer->createRenderTarget(rt_desc);

		// create default full-window viewport
		if (m_viewports.empty()) {
			createViewport("default", 0, 0, m_width, m_height);
		}
	}	

	void Window::onResize(int width, int height)
	{
		if (m_width == width && m_height == height)
			return;

		m_width = width;
		m_height = height;

		if (!m_render_target || !m_swap_chain) {
			LOG(LOG_ERROR) << "Window::onResize: no render target or swap chain";
			return;
		}

		m_render_target->Release();
		m_swap_chain->onResize(width, height);

		hippios::RenderTargetDesc rt_desc{};
		rt_desc.type = hippios::RenderTargetType::SwapChain;
		rt_desc.width = width;
		rt_desc.height = height;
		rt_desc.swap_chain = m_swap_chain.get();
		m_render_target = m_renderer->createRenderTarget(rt_desc);

		for (auto& vp : m_viewports) {
			// simple policy: fullscreen viewport tracks window size if it filled the old window
			if (vp->getX() == 0 && vp->getY() == 0 &&
				vp->getWidth() == m_width && vp->getHeight() == m_height) {
				vp->setRect(0, 0, width, height);
			}
		}
	}

	void Window::SetFullscreen(bool mode)
	{
		bool exclusive_fullscreen = false;
		if (exclusive_fullscreen && m_swap_chain)
			m_swap_chain->SetFullscreen(mode);
		else if (m_native_win)
			m_native_win->SetFullscreen(mode);
	}

	Viewport* Window::createViewport(const std::string& name, int x, int y, int width, int height)
	{
		auto vp = std::make_unique<Viewport>(name);
		vp->setWindow(this);
		vp->setRect(x, y, width, height);

		Viewport* raw = vp.get();
		m_viewports.push_back(std::move(vp));
		return raw;
	}

	void Window::addViewport(std::unique_ptr<Viewport> viewport)
	{
		if (!viewport)
			return;

		viewport->setWindow(this);
		m_viewports.push_back(std::move(viewport));
	}

	void Window::removeViewport(Viewport* viewport)
	{
		m_viewports.erase(
			std::remove_if(m_viewports.begin(), m_viewports.end(),
				[&](const std::unique_ptr<Viewport>& vp) { return vp.get() == viewport; }),
			m_viewports.end());
	}

	Viewport* Window::getViewport(const std::string& name)
	{
		for (const auto& vp : m_viewports) {
			if (vp->isName(name))
				return vp.get();
		}
		return nullptr;
	}
}
