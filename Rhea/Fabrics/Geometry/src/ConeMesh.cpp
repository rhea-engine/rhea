/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#include <Rhea/Geometry/ConeMesh.h>
#include <Rhea/Geometry/MeshBuilder.h>
#include <Math/Math.h>

namespace rhea
{
	void ConeMesh::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
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
		if (values.find("cap") != values.end())
			setCap(values["cap"].toBool());
	}

    void ConeMesh::createMesh()
    {
        MeshBuilder b;

        const float r = m_radius;
        const float h = m_height * 0.5f;
        const uint32_t segments = std::max(3u, m_segments);

        // Side
        for (uint32_t i = 0; i <= segments; ++i) {
            float u = static_cast<float>(i) / static_cast<float>(segments);
            float a = u * math::pi * 2.0f;

            float x = std::cos(a);
            float z = std::sin(a);

            math::Vector3 basePos = { x * r, -h, z * r };
            math::Vector3 tipPos = { 0.0f,  h, 0.0f };

            // Smooth cone side normal.
            // For cone y-up: normal roughly points outward and upward.
            math::Vector3 normal = math::Vector3{
                x * m_height,
                r,
                z * m_height
                }.normalizedCopy();

            b.addStandardVertex(basePos, normal, { u, 1.0f });
            b.addStandardVertex(tipPos, normal, { u, 0.0f });
        }

        for (uint32_t i = 0; i < segments; ++i) {
            uint32_t base0 = i * 2;
            uint32_t tip0 = base0 + 1;
            uint32_t base1 = base0 + 2;
            uint32_t tip1 = base0 + 3;

            // only one triangle per segment
            b.addTriangle(base0, base1, tip0);
        }

        if (m_cap) {
            uint32_t center = b.vertexCount();
            b.addStandardVertex(
                { 0.0f, -h, 0.0f },
                { 0.0f, -1.0f, 0.0f },
                { 0.5f, 0.5f }
            );

            uint32_t start = b.vertexCount();

            for (uint32_t i = 0; i <= segments; ++i) {
                float u = static_cast<float>(i) / static_cast<float>(segments);
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
                b.addTriangle(center, start + i + 1, start + i);
            }
        }

        b.generateTangents();
        b.buildInto(*this);
    }
}