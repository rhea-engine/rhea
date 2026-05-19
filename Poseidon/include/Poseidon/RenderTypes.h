/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef POSEIDON_RENDERTYPES_H
#define POSEIDON_RENDERTYPES_H

#include <cstdint>

namespace poseidon
{
    using RenderableHandle = uint32_t;
    using LightHandle = uint32_t;
    using CameraHandle = uint32_t;

    constexpr RenderableHandle InvalidRenderable = 0;
    constexpr LightHandle InvalidLight = 0;
    constexpr CameraHandle InvalidCamera = 0;

    struct ClipRect
    {
        float x;
        float y;
        float width;
        float height;
    };

    inline bool operator!=(const ClipRect& a, const ClipRect& b) {
        return a.x != b.x || a.y != b.y || a.width != b.width || a.height != b.height;
    }
}

#endif // !POSEIDON_RENDERTYPES_H