#include "RenderQueue.h"

namespace Poseidon
{
	void RenderQueue::addRenderable(Renderable* renderable)
	{
		m_render_items.push_back(renderable);
	}

	void RenderQueue::clear()
	{
		m_render_items.clear();
	}

	Renderable* RenderQueue::first()
	{
		it = m_render_items.begin();
		if (it != m_render_items.end())
			return *it;
	}

	Renderable* RenderQueue::next()
	{
		++it;
		if (it != m_render_items.end())
			return *it;
		else
			return NULL;
	}
}