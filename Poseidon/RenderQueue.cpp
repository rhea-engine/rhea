#include "RenderQueue.h"

namespace Poseidon
{
	void RenderQueue::addRenderable(Item* renderable)
	{
		m_render_items.push_back(renderable);
	}

	void RenderQueue::clear()
	{
		m_render_items.clear();
	}

	Item* RenderQueue::first()
	{
		it = m_render_items.begin();
		if (it != m_render_items.end())
			return *it;
		return nullptr;
	}

	Item* RenderQueue::next()
	{
		++it;
		if (it != m_render_items.end())
			return *it;
		else
			return NULL;
	}
}