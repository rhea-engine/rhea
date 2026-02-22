#ifndef RHEA_PHYSICS_PHYSICSBODYDESC_H
#define RHEA_PHYSICS_PHYSICSBODYDESC_H

#include <string>
//#include <unordered_map>
//#include <Rhea/Unit.h>

namespace Rhea
{
    struct PhysicsBodyDesc
    {
        std::string kind;        // e.g. "Meworla/RheaPhysics:Character"
        float mass = 0.0f;       // 0 => static by default
        bool kinematic = false;
        bool trigger = false;

        // Optional: filtering
        uint16_t group = 1;
        uint16_t mask = 0xFFFF;

        // Optional: per-item overrides (string->variant)
        // If you already have Variant: use that instead of reinventing.
        //std::unordered_map<std::string, Variant> params;
    };
}
#endif // !RHEA_PHYSICS_PHYSICSBODYDESC_H
