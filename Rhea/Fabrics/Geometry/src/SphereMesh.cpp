/** 
 * Copyright (c) 2019-2026 Meworla
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#define NOMINMAX
#include <Math/Math.h>
#include <cmath>
#include <algorithm>

#include <Rhea/Geometry/SphereMesh.h>
#include <Math/Vector2.h>
#include <Math/Vector3.h>
#include <Rhea/Geometry/MeshBuilder.h>

namespace rhea
{
    void SphereMesh::createMesh()
    {
        MeshBuilder b;

        const float radius = m_radius;
        const uint32_t segments = std::max(3u, m_segments);
        const uint32_t rings = std::max(2u, m_rings);

        for (uint32_t y = 0; y <= rings; ++y) {
            float v = static_cast<float>(y) / static_cast<float>(rings);
            float theta = v * math::M_PI; // 0 -> pi

            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);

            for (uint32_t x = 0; x <= segments; ++x) {
                float u = static_cast<float>(x) / static_cast<float>(segments);
                float phi = u * math::M_PI * 2.0f;

                float sinPhi = std::sin(phi);
                float cosPhi = std::cos(phi);

                math::Vector3 normal = {
                    sinTheta * cosPhi,
                    cosTheta,
                    sinTheta * sinPhi
                };

                math::Vector3 position = normal * radius;
                math::Vector2 uv = { u, v };

                b.addStandardVertex(position, normal, uv);
            }
        }

        const uint32_t stride = segments + 1;

        for (uint32_t y = 0; y < rings; ++y) {
            for (uint32_t x = 0; x < segments; ++x) {
                uint32_t i0 = y * stride + x;
                uint32_t i1 = i0 + 1;
                uint32_t i2 = i0 + stride;
                uint32_t i3 = i2 + 1;

                b.addQuadFlipped(i0, i1, i3, i2);
            }
        }

        b.generateTangents();
        b.buildInto(*this);
    }
}