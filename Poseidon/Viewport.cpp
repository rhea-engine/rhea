#include "Viewport.h"
#include "Mesh.h"
#include "Animatable.h"

#include "Entity.h"
#include "Hippios/D3D11Renderer/D3D11Renderer.h"

namespace Poseidon
{	
	Viewport::Viewport()
	{
		

	}
	
	bool Viewport::render()
	{
		// clear pass
		m_renderer->clearPass();

		// TODO is this the model pass?
		((Hippios::D3D11Renderer*)m_renderer)->update();

		// need a camera to do anything
		if (!m_camera)
			return false;

		// render pass
		// my children
		for (std::vector<Animatable*>::iterator it = m_render_children.begin(); it != m_render_children.end(); ++it)
		{
			Animatable* robj = (Animatable*)*it;
			if (robj->render(this, m_camera))
				return true;
		}

		// present pass
		if (m_renderer->presentPass())
			return true;
		else
			return false;
	}

	void Viewport::initRenderTarget(Hippios::Engine* engine)
	{
		m_engine = engine;
		m_renderer = m_engine->getRenderer();
	}

	void Viewport::setCamera(Camera* camera)
	{
		m_camera = camera;
	}

	void Viewport::addRenderChild(Animatable* child)
	{
		Node::addChild(child);
		m_render_children.push_back(child);
	}
}