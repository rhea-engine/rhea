
#ifndef POSEIDON_LIGHT_H
#define POSEIDON_LIGHT_H

#include "RenderTypes.h"
#include <Math/Quaternion.h>
#include <Math/Vector3.h>
#include <Rhea/Core/Light.h>

namespace poseidon
{
    enum class LightType
    {
        Directional,
        Point,
        Spot
    };

    struct Light
    {
        LightHandle handle = InvalidLight;
        LightType type = LightType::Point;

        math::Vector3 direction;
        math::Quaternion position;

        math::Vector3 color;
        math::Quaternion diffuse;
        math::Quaternion specular;

        float coneAngle = 0.0f;
        float constantAttenuation = 1.0f;
        float linearAttenuation = 0.0f;
        float quadraticAttenuation = 0.0f;

        bool visible = true;

        rhea::Light* sourceLight = nullptr;
    };
}

#endif // !POSEIDON_LIGHT_H
