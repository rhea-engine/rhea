#include "Engine.h"

#include "Renderer.h"
#include "D3D11Renderer/D3D11Renderer.h"
#include "D3D11Renderer/D3D11Window.h"

namespace Hippios
{
	void Engine::startup()
	{
		m_renderer = (Renderer *)new D3D11Renderer();		
	}	

	HippiosWindow* Engine::createWindow()
	{
		// creates OS and render window
		m_window = m_renderer->createWindow();
		return m_window;
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

	void Engine::setRenderer(Renderer* renderer)
	{
		m_renderer = renderer;
	}
}