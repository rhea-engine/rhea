/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef POSEIDON_RENDERQUEUE_H
#define POSEIDON_RENDERQUEUE_H

#include <vector>

#include "RenderTypes.h"

namespace poseidon
{
	class RenderQueue
	{
    public:
        void addRenderable(RenderableHandle h);
        void clear();

        RenderableHandle first();
        RenderableHandle next();

    private:
        std::vector<RenderableHandle> m_renderables;
        int m_idx = -1;
	};
}

#endif // !POSEIDON_RENDERQUEUE_H
