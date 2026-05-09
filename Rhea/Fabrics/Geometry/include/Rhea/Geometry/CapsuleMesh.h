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

        void setRadius(float radius) { m_radius = radius; markDirty(); }
        void setHeight(float height) { m_height = height; markDirty(); } // total height, including hemispheres
        void setSegments(uint32_t segments) { m_segments = segments; markDirty(); }
        void setRings(uint32_t rings) { m_rings = rings; markDirty(); }

        void createMesh();

    private:
        float m_radius = 0.5f;
        float m_height = 2.0f;
        uint32_t m_segments = 32;
        uint32_t m_rings = 8;
    };
}
#endif // !RHEA_GEOMETRY_CAPSULEMESH_H
