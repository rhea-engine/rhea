/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#define NOMINMAX
#include <Rhea/Geometry/CapsuleMesh.h>
#include <Rhea/Geometry/MeshBuilder.h>
#include <Math/Math.h>

#include <cmath>

namespace rhea
{
	void CapsuleMesh::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		GeometryMesh::setup(app, attributes, values);
		if (values.find("radius") != values.end())
			setRadius(values["radius"].toFloat());
		if (values.find("height") != values.end())
			setHeight(values["height"].toFloat());
		if (values.find("segments") != values.end()) {
            const int segments = values["segments"].toInt();
            setSegments(static_cast<uint32_t>(std::max(3, segments)));
        }
		if (values.find("rings") != values.end()) {
            const int rings = values["rings"].toInt();
            setRings(static_cast<uint32_t>(std::max(2, rings)));
        }
	}

    void CapsuleMesh::createMesh()
    {
        MeshBuilder b;

        const float r = m_radius;
        const float totalH = std::max(m_height, 2.0f * r);
        const float cylH = totalH - 2.0f * r;
        const float halfCyl = cylH * 0.5f;

        const uint32_t segments = std::max(3u, m_segments);
        const uint32_t rings = std::max(2u, m_rings);

        // rings go from bottom pole to top pole
        const uint32_t totalRings = rings * 2;

        for (uint32_t y = 0; y <= totalRings; ++y) {
            float t = static_cast<float>(y) / static_cast<float>(totalRings);

            // angle from bottom pole to top pole
            float theta = -math::M_PI * 0.5f + t * math::M_PI;

            float cosTheta = std::cos(theta);
            float sinTheta = std::sin(theta);

            bool top = sinTheta >= 0.0f;

            float centerY = top ? halfCyl : -halfCyl;

            for (uint32_t x = 0; x <= segments; ++x) {
                float u = static_cast<float>(x) / static_cast<float>(segments);
                float phi = u * math::M_PI * 2.0f;

                float cp = std::cos(phi);
                float sp = std::sin(phi);

                math::Vector3 normal = {
                    cosTheta * cp,
                    sinTheta,
                    cosTheta * sp
                };

                math::Vector3 position = {
                    normal.x * r,
                    centerY + normal.y * r,
                    normal.z * r
                };

                b.addStandardVertex(position, normal, { u, t });
            }
        }

        uint32_t stride = segments + 1;

        for (uint32_t y = 0; y < totalRings; ++y) {
            for (uint32_t x = 0; x < segments; ++x) {
                uint32_t i0 = y * stride + x;
                uint32_t i1 = i0 + 1;
                uint32_t i2 = i0 + stride;
                uint32_t i3 = i2 + 1;

                b.addQuad(i0, i1, i3, i2);
            }
        }

        b.generateTangents();
        b.buildInto(*this);
    }
}