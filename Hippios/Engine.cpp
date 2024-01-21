#include "Engine.h"

#include "Renderer.h"
#include "D3D11Renderer/D3D11Renderer.h"
#include "D3D11Renderer/D3D11Window.h"

namespace Hippios
{
	void Engine::startup()
	{
		m_renderer = std::make_unique<D3D11Renderer>();		
	}	

	HippiosWindow* Engine::createWindow()
	{
		// creates OS and render window
		HippiosWindow* win = m_renderer->createWindow(m_psio);
		if (!m_main_window)
			m_main_window = win; // set main window for first window
		return win;
	}

	bool Engine::render()
	{
		m_renderer->render();
		return false;
	}

	void Engine::update(VertexBuffer* vb, IndexBuffer* ib)
	{
		m_renderer->bindBuffers(vb, ib);
	}

	void Engine::setRenderer(std::unique_ptr<Renderer> renderer)
	{
		m_renderer = std::move(renderer);
	}
}