#include "Viewport.h"
#include "Mesh.h"

namespace Poseidon
{	
	bool Viewport::onUpdate()
	{
		// clear pass
		m_renderer->clearPass();

		// need a camera to do anything
		if (!m_camera)
			return false;

		// render pass
		// my children
		for (std::vector<Mesh*>::iterator it = m_render_children.begin(); it != m_render_children.end(); ++it)
		{
			Mesh* mesh = (Mesh*)*it;
			if (mesh->render(m_camera))
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
		m_win = m_engine->createWindow();
		m_renderer = m_engine->getRenderer();		
	}

	void Viewport::setCamera(Camera* camera)
	{
		m_camera = camera;
	}

	void Viewport::addRenderChild(Mesh* mesh)
	{
		m_render_children.push_back(mesh);
	}
}