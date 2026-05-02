/*
* Copyright (c) 2019-2026 Meworla
*
* Licensed under the MIT License.
* See LICENSE file in the project root for full license information.
*/
#ifndef RHEA_GEOMETRY_TORUSMESH_H
#define RHEA_GEOMETRY_TORUSMESH_H

#include <Rhea/Geometry/GeometryMesh.h>
#define NOMINMAX
#include <algorithm>
#include <cmath>

namespace rhea
{
    class TorusMesh : public GeometryMesh {
        RECLASS(TorusMesh, GeometryMesh, "//meworla/torusmesh")
    public:
        TorusMesh(std::string name, std::string type_name) : GeometryMesh(name, type_name) {}

        void setMajorRadius(float radius) {
            m_majorRadius = (std::max)(0.001f, radius);
            markDirty();
        }

        void setMinorRadius(float radius) {
            m_minorRadius = (std::max)(0.001f, radius);
            markDirty();
        }

        void setSegments(uint32_t segments) {
            m_segments = (std::max)(3u, segments);
            markDirty();
        }

        void setTubeSegments(uint32_t segments) {
            m_tubeSegments = (std::max)(3u, segments);
            markDirty();
        }

        void setResolution(uint32_t segments, uint32_t tubeSegments) {
            setSegments(segments);
            setTubeSegments(tubeSegments);
        }

		void createMesh() override;

    private:
        float m_majorRadius = 1.0f;
        float m_minorRadius = 0.25f;
        uint32_t m_segments = 48;
        uint32_t m_tubeSegments = 16;
    };
}
#endif // RHEA_GEOMETRY_TORUSMESH_H