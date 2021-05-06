#include "RenderContext.h"
#include "Renderable.h"
#include "RenderQueue.h"
#include "Camera.h"

namespace Poseidon
{
	RenderContext::RenderContext(std::string& name) :
		m_camera(nullptr),
		m_name(name),
		m_stencil_state(0)
	{
	}

	void RenderContext::cullObjects(RenderQueue* queue)
	{
		// cull pass
		// my children
		for (std::vector<Renderable*>::iterator it = m_render_children.begin(); it != m_render_children.end(); ++it)
		{
			Renderable* robj = (Renderable*)*it;
			robj->cullPhase(queue, m_camera);
		}
	}

	void RenderContext::setCamera(Camera* camera)
	{
		m_camera = camera;
	}

	void RenderContext::addRenderChild(Renderable* child)
	{
		Node::addChild(child);
		m_render_children.push_back(child);
	}

	void RenderContext::removeRenderChild(Renderable* child)
	{
		auto it = std::find(m_render_children.begin(), m_render_children.end(), child);
		m_render_children.erase(it);
	}
}
