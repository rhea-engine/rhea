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
}

#endif // !POSEIDON_RENDERTYPES_H