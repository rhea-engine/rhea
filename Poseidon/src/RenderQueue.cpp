/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Poseidon/RenderQueue.h"
#include <Poseidon/RenderWorld.h>
#include <Poseidon/Renderable.h>
#include <algorithm>

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

	void RenderQueue::sortPhase(RenderWorld* world)
	{
		std::stable_sort(
			m_renderables.begin(),
			m_renderables.end(),
			[world](RenderableHandle a, RenderableHandle b)
			{
				const auto* ra = world->resolveRenderable(a);
				const auto* rb = world->resolveRenderable(b);

				return ra->renderPhase < rb->renderPhase;
			});
	}
}