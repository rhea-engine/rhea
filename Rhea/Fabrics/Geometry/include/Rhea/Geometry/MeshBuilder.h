/*
* Copyright (c) 2019-2026 Meworla
* Licensed under the MIT License.
* See LICENSE file in the project root for full license information.
*/

#ifndef RHEA_GEOMETRY_MESHBUILDER_H
#define RHEA_GEOMETRY_MESHBUILDER_H

#include <vector>
#include <cstdint>
#include <string>

#include <Rhea/Resources/VertexBufferStandard.h>
#include <Rhea/Core/Mesh.h>

namespace rhea {

    class MeshBuilder {
    public:
        MeshBuilder() = default;

        uint32_t vertexCount() const {
            return static_cast<uint32_t>(m_vertices.size());
        }

        MeshBuilder& addVertex(const VertexBufferStandard::VertexStandard& v) {
            m_vertices.push_back(v);
            return *this;
        }

        MeshBuilder& addStandardVertex(const math::Vector3& position, const math::Vector3& normal, const math::Vector2& uv);

        MeshBuilder& addIndex(uint32_t i) {
            m_indices.push_back(i);
            return *this;
        }

        MeshBuilder& addTriangle(uint32_t a, uint32_t b, uint32_t c) {
            m_indices.push_back(a);
            m_indices.push_back(b);
            m_indices.push_back(c);
            return *this;
        }

        MeshBuilder& addQuadFlipped(uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
            addTriangle(a, b, c);
            addTriangle(a, c, d);
            return *this;
        }

        // Vertices are expected in clockwise order as seen from outside.
        // Produces triangles with engine front-face winding.
        MeshBuilder& addQuad(uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
        	addTriangle(a, c, b);
			addTriangle(a, d, c);
            return *this;
        }

        MeshBuilder& generateTangents();

        void buildInto(Mesh& mesh);

    private:
        std::vector<VertexBufferStandard::VertexStandard> m_vertices;
        std::vector<uint32_t> m_indices;
    };

}
#endif // !RHEA_GEOMETRY_MESHBUILDER_H