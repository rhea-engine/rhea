/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#ifndef RHEA_GEOMETRY_CAPSULEMESH_H
#define RHEA_GEOMETRY_CAPSULEMESH_H

#include <Rhea/Geometry/GeometryMesh.h>

namespace rhea
{
    class CapsuleMesh :public GeometryMesh
    {
        RECLASS(CapsuleMesh, GeometryMesh, "//meworla/capsulemesh")
    public:
        CapsuleMesh(std::string name, std::string type_name) : GeometryMesh(name, type_name) {}
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;

        void setRadius(float radius) { m_radius = (std::max)(0.001f, radius); markDirty(); }
        void setHeight(float height) { m_height = (std::max)(0.001f, height); markDirty(); } // total height, including hemispheres
        void setSegments(uint32_t segments) { m_segments = (std::max)(3u, segments); markDirty(); }
        void setRings(uint32_t rings) { m_rings = (std::max)(3u, rings); markDirty(); }

        void createMesh();

    private:
        float m_radius = 0.5f;
        float m_height = 2.0f;
        uint32_t m_segments = 32;
        uint32_t m_rings = 8;
    };
}
#endif // !RHEA_GEOMETRY_CAPSULEMESH_H
