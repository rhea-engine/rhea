#include "Engine.h"

#include "Renderer.h"
#include "D3D11Renderer/D3D11Renderer.h"

namespace Hippios
{
	void Engine::startup()
	{
		m_renderer = std::make_unique<D3D11Renderer>();		
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