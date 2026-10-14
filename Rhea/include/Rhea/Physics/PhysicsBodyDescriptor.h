/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_PHYSICS_PHYSICSBODYDESC_H
#define RHEA_PHYSICS_PHYSICSBODYDESC_H

#include <string>
#include <cstdint>

namespace rhea
{
    struct PhysicsBodyDesc
    {
        std::string kind;        // e.g. "Meworla/RheaPhysics:Character"
        float mass = 0.0f;       // 0 => static by default
        bool kinematic = false;
        bool trigger = false;

        // filtering
        uint16_t group = 1;
        uint16_t mask = 0xFFFF;
    };
}
#endif // !RHEA_PHYSICS_PHYSICSBODYDESC_H
