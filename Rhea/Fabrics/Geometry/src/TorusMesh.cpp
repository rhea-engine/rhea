/*
* Copyright (c) 2019-2026 Meworla
*
* Licensed under the MIT License.
* See LICENSE file in the project root for full license information.
*/
#include <Rhea/Geometry/TorusMesh.h>
#include <Rhea/Geometry/MeshBuilder.h>
#include <Math/Math.h>

namespace rhea
{
	void TorusMesh::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		GeometryMesh::setup(app, attributes, values);
		if (values.find("majorRadius") != values.end())
			setMajorRadius(values["majorRadius"].toFloat());
		if (values.find("minorRadius") != values.end())
			setMinorRadius(values["minorRadius"].toFloat());
		if (values.find("segments") != values.end()) {
			const int segments = values["segments"].toInt();
			setSegments(static_cast<uint32_t>(std::max(3, segments)));
		}
		if (values.find("tubeSegments") != values.end()) {
			const int tubeSegments = values["tubeSegments"].toInt();
			setTubeSegments(static_cast<uint32_t>(std::max(3, tubeSegments)));
		}
	}

    void TorusMesh::createMesh()
    {
        MeshBuilder b;

        const float majorR = std::max(0.001f, m_majorRadius);
        const float minorR = std::max(0.001f, m_minorRadius);

        const uint32_t segments = std::max(3u, m_segments);
        const uint32_t tubeSegments = std::max(3u, m_tubeSegments);

        for (uint32_t i = 0; i <= segments; ++i) {
            float u = static_cast<float>(i) / static_cast<float>(segments);
            float a = u * math::M_PI * 2.0f;

            float ca = std::cos(a);
            float sa = std::sin(a);

            math::Vector3 ringCenter = {
                majorR * ca,
                0.0f,
                majorR * sa
            };

            for (uint32_t j = 0; j <= tubeSegments; ++j) {
                float v = static_cast<float>(j) / static_cast<float>(tubeSegments);
                float bAngle = v * math::M_PI * 2.0f;

                float cb = std::cos(bAngle);
                float sb = std::sin(bAngle);

                math::Vector3 normal = {
                    ca * cb,
                    sb,
                    sa * cb
                };

                math::Vector3 position = ringCenter + normal * minorR;

                b.addStandardVertex(position, normal, { u, v });
            }
        }

        const uint32_t stride = tubeSegments + 1;

        for (uint32_t i = 0; i < segments; ++i) {
            for (uint32_t j = 0; j < tubeSegments; ++j) {
                uint32_t i0 = i * stride + j;
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