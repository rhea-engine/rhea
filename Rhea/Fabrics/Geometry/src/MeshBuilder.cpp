/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Geometry/MeshBuilder.h>
#include <Math/Vector3.h>
#include <Math/Vector2.h>
#include <Math/Vector4.h>
#include <Math/Matrix4.h>

namespace rhea
{
    MeshBuilder& MeshBuilder::generateTangents()
    {
        std::vector<math::Vector3> tangents(m_vertices.size(), math::Vector3{ 0, 0, 0 });

        for (size_t i = 0; i + 2 < m_indices.size(); i += 3) {
            uint32_t i0 = m_indices[i + 0];
            uint32_t i1 = m_indices[i + 1];
            uint32_t i2 = m_indices[i + 2];

            auto& v0 = m_vertices[i0];
            auto& v1 = m_vertices[i1];
            auto& v2 = m_vertices[i2];

            math::Vector3 e1 = v1.position - v0.position;
            math::Vector3 e2 = v2.position - v0.position;

            math::Vector2 duv1 = v1.texcoord - v0.texcoord;
            math::Vector2 duv2 = v2.texcoord - v0.texcoord;

            float denom = duv1.x * duv2.y - duv2.x * duv1.y;
            if (std::abs(denom) < 0.000001f)
                continue;

            float r = 1.0f / denom;

            math::Vector3 tangent = {
                r * (duv2.y * e1.x - duv1.y * e2.x),
                r * (duv2.y * e1.y - duv1.y * e2.y),
                r * (duv2.y * e1.z - duv1.y * e2.z)
            };

            tangents[i0] += tangent;
            tangents[i1] += tangent;
            tangents[i2] += tangent;
        }

        for (size_t i = 0; i < m_vertices.size(); ++i) {
            math::Vector3 n = m_vertices[i].normal.normalizedCopy();
            math::Vector3 t = tangents[i];

            // Gram-Schmidt orthogonalize
            t = (t - n * n.dotProduct(t)).normalizedCopy();

            m_vertices[i].tangent = math::Vector3{ t.x, t.y, t.z };
        }

        return *this;
    }

    void MeshBuilder::buildInto(Mesh& mesh)
    {
        auto* vb = new VertexBufferStandard();

        for (const auto& v : m_vertices) {
            vb->push_back(v);
        }

        mesh.setVertexBuffer(*vb);
        mesh.addSubmesh(nullptr, m_indices);
    }

    MeshBuilder& MeshBuilder::addStandardVertex(const math::Vector3& position, const math::Vector3& normal, const math::Vector2& uv)
    {
        // initialize tangent safely
        m_vertices.push_back({ position, normal, uv,  math::Vector3{0.0f, 0.0f, 0.0f} });
        return *this;
    }
}