#include "Poseidon/RenderQueue.h"

namespace poseidon
{
	void RenderQueue::addRenderable(RenderableHandle h)
	{
		m_renderables.push_back(h);
	}

	void RenderQueue::clear()
	{
		m_renderables.clear();
		m_idx = -1;
	}

	RenderableHandle RenderQueue::first()
	{
		m_idx = 0;
		if (m_idx < static_cast<int>(m_renderables.size()))
			return m_renderables[m_idx];
		return InvalidRenderable;
	}

	RenderableHandle RenderQueue::next()
	{
		++m_idx;
		if (m_idx < static_cast<int>(m_renderables.size()))
			return m_renderables[m_idx];
		return InvalidRenderable;
	}
}