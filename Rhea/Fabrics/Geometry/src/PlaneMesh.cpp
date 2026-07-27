/*  
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Geometry/PlaneMesh.h>
#include <Rhea/Geometry/MeshBuilder.h>

namespace rhea
{
    void PlaneMesh::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
    {
		GeometryMesh::setup(app, attributes, values);

		if (values.find("width") != values.end())   
			m_width = (std::max)(0.001f, values["width"].toFloat());
		if (values.find("height") != values.end())
			m_height = (std::max)(0.001f, values["height"].toFloat());
		if (values.find("segmentsX") != values.end())
			m_segmentsX = (std::max)(1, values["segmentsX"].toInt());
		if (values.find("segmentsY") != values.end())
			m_segmentsY = (std::max)(1, values["segmentsY"].toInt());
		if (values.find("centered") != values.end())
			m_centered = values["centered"].toBool();
		if (values.find("uvScale") != values.end()) {
            if (values["uvScale"].isArray() && values["uvScale"].length() == 2)
                m_uvScale = math::Vector2((std::max)(0.001f, values["uvScale"][0].toFloat()), (std::max)(0.001f, values["uvScale"][1].toFloat()));
        }
    }

    void PlaneMesh::createMesh()
    {
        MeshBuilder b;

        const float halfW = m_width * 0.5f;
        const float halfH = m_height * 0.5f;

        for (uint32_t y = 0; y <= m_segmentsY; ++y) {
            float v = static_cast<float>(y) / m_segmentsY;
            for (uint32_t x = 0; x <= m_segmentsX; ++x) {
                float u = static_cast<float>(x) / m_segmentsX;

                float px = u * m_width;
                float pz = v * m_height;

                if (m_centered) {
                    px -= halfW;
                    pz -= halfH;
                }

                math::Vector3 position = { px, 0.0f, pz };
                math::Vector3 normal = { 0.0f, 1.0f, 0.0f };
                math::Vector2 uv = { u * m_uvScale.x, v * m_uvScale.y };

                b.addStandardVertex(position, normal, uv);
            }
        }

        // indices
        int stride = m_segmentsX + 1;

        for (uint32_t y = 0; y < m_segmentsY; ++y) {
            for (uint32_t x = 0; x < m_segmentsX; ++x) {

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