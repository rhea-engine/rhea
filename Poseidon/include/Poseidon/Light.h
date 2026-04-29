/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */


#ifndef POSEIDON_LIGHT_H
#define POSEIDON_LIGHT_H

#include "RenderTypes.h"
#include <Math/Quaternion.h>
#include <Math/Vector3.h>

namespace poseidon
{
    

    class Light
    {
    public:
        enum class LightType
        {
            Directional,
            Point,
            Spot
        };
        
        void setType(LightType type) { m_type = type; }
        LightType getType() const { return m_type; }

        void setVisible(bool v) { m_visible = v; }
        bool isVisible() const { return m_visible; }

        void setColor(const math::Vector3& c) { m_color = c; }
        const math::Vector3& getColor() const { return m_color; }

        void setDiffuse(const math::Quaternion& d) { m_diffuse = d; }
        const math::Quaternion& getDiffuse() const { return m_diffuse; }

        void setSpecular(const math::Quaternion& s) { m_specular = s; }
        const math::Quaternion& getSpecular() const { return m_specular; }

        void setPosition(const math::Vector3& p) { m_position = p; }
        const math::Vector3& getPosition() const { return m_position; }

        void setDirection(const math::Vector3& d) { m_direction = d; }
        const math::Vector3& getDirection() const { return m_direction; }

        void setAttenuation(const math::Vector3& a) { m_attenuation = a; }
        const math::Vector3& getAttenuation() const { return m_attenuation; }

        void setConeAngle(float angle) { m_coneAngle = angle; }
        float getConeAngle() const { return m_coneAngle; }

    private:
        LightType m_type = LightType::Point;
        bool m_visible = true;

        math::Vector3 m_color = math::Vector3::UNIT_SCALE;
        math::Quaternion m_diffuse;
        math::Quaternion m_specular;

        math::Vector3 m_position = math::Vector3::ZERO;
        math::Vector3 m_direction = math::Vector3::UNIT_Z;

        math::Vector3 m_attenuation = math::Vector3(1.0f, 0.0f, 0.0f);
        float m_coneAngle = 0.0f;
    };
}

#endif // !POSEIDON_LIGHT_H
