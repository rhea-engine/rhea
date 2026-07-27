/*
* Copyright (c) 2019-2026 Meworla
* 
* Licensed under the MIT License.
* See LICENSE file in the project root for full license information.
*/
#ifndef RHEA_SPHEREMESH_H
#define RHEA_SPHEREMESH_H

#define NOMINMAX
#include <algorithm>
#include <cmath>
#include <Rhea/Geometry/GeometryMesh.h>

namespace rhea
{
    class SphereMesh : public GeometryMesh {
        RECLASS(SphereMesh, GeometryMesh, "//meworla/spheremesh")
    public:
        SphereMesh(std::string name, std::string type_name) : GeometryMesh(name, type_name) {}
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;

        void setRadius(float radius) { m_radius = (std::max)(0.001f, radius); markDirty(); }
        void setSegments(uint32_t segments) { m_segments = (std::max)(3u, segments); markDirty(); }
        void setRings(uint32_t rings) { m_rings = (std::max)(2u, rings); markDirty(); }

        void setResolution(uint32_t segments, uint32_t rings) {
            setSegments(segments);
            setRings(rings);
        }

		void createMesh() override;

    private:
        float m_radius = 1.0f;
        uint32_t m_segments = 32;
        uint32_t m_rings = 16;
    };
}
#endif // !RHEA_SPHEREMESH_H
