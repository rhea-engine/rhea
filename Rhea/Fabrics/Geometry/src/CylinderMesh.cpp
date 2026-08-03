/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#define NOMINMAX
#include <Math/Math.h>
#include <Math/Vector3.h>
#include <cmath>
#include <algorithm>

#include <Rhea/Geometry/CylinderMesh.h>
#include <Rhea/Geometry/MeshBuilder.h>

namespace rhea
{
	void CylinderMesh::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
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
		if (values.find("caps") != values.end())
			setCaps(values["caps"].toBool());
	}

    void CylinderMesh::createMesh()
    {
        MeshBuilder b;

        const float r = m_radius;
        const float h = m_height * 0.5f;
        const uint32_t segments = std::max(3u, m_segments);

        // side vertices
        for (uint32_t i = 0; i <= segments; ++i) {
            float u = static_cast<float>(i) / segments;
            float a = u * math::pi * 2.0f;

            float x = std::cos(a);
            float z = std::sin(a);

            math::Vector3 normal = { x, 0.0f, z };

            b.addStandardVertex({ x * r, -h, z * r }, normal, { u, 1.0f });
            b.addStandardVertex({ x * r,  h, z * r }, normal, { u, 0.0f });
        }

        // side indices
        for (uint32_t i = 0; i < segments; ++i) {
            uint32_t i0 = i * 2;
            uint32_t i1 = i0 + 1;
            uint32_t i2 = i0 + 2;
            uint32_t i3 = i0 + 3;

            b.addQuad(i0, i2, i3, i1);
        }

        if (m_caps) {
            // bottom cap
            uint32_t bottomCenter = b.vertexCount();
            b.addStandardVertex({ 0.0f, -h, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.5f, 0.5f });

            uint32_t bottomStart = b.vertexCount();

            for (uint32_t i = 0; i <= segments; ++i) {
                float u = static_cast<float>(i) / segments;
                float a = u * math::pi * 2.0f;

                float x = std::cos(a);
                float z = std::sin(a);

                b.addStandardVertex(
                    { x * r, -h, z * r },
                    { 0.0f, -1.0f, 0.0f },
                    { x * 0.5f + 0.5f, z * 0.5f + 0.5f }
                );
            }

            for (uint32_t i = 0; i < segments; ++i) {
                b.addTriangle(bottomCenter, bottomStart + i + 1, bottomStart + i);
            }

            // top cap
            uint32_t topCenter = b.vertexCount();
            b.addStandardVertex({ 0.0f, h, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.5f, 0.5f });

            uint32_t topStart = b.vertexCount();

            for (uint32_t i = 0; i <= segments; ++i) {
                float u = static_cast<float>(i) / segments;
                float a = u * math::pi * 2.0f;

                float x = std::cos(a);
                float z = std::sin(a);

                b.addStandardVertex(
                    { x * r, h, z * r },
                    { 0.0f, 1.0f, 0.0f },
                    { x * 0.5f + 0.5f, z * 0.5f + 0.5f }
                );
            }

            for (uint32_t i = 0; i < segments; ++i) {
                b.addTriangle(topCenter, topStart + i + 1, topStart + i);
            }
        }

        b.generateTangents();
        b.buildInto(*this);
    }
}