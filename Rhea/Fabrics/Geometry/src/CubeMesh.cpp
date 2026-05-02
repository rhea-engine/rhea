/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Geometry/CubeMesh.h>
#include <Rhea/Geometry/MeshBuilder.h>

namespace rhea
{
	void CubeMesh::createMesh()
	{
        MeshBuilder b;

        float h = m_size * 0.5f;

        struct Face {
            math::Vector3 normal;
            math::Vector3 v[4];
        };

        Face faces[6] = {
            // +X
            { {1,0,0}, {
                { h,-h,-h}, { h,-h, h}, { h, h, h}, { h, h,-h}
            }},
            // -X
            { {-1,0,0}, {
                {-h,-h, h}, {-h,-h,-h}, {-h, h,-h}, {-h, h, h}
            }},
            // +Y
            { {0,1,0}, {
                {-h, h,-h}, { h, h,-h}, { h, h, h}, {-h, h, h}
            }},
            // -Y
            { {0,-1,0}, {
                {-h,-h, h}, { h,-h, h}, { h,-h,-h}, {-h,-h,-h}
            }},
            // +Z
            { {0,0,1}, {
                {-h,-h, h}, {-h, h, h}, { h, h, h}, { h,-h, h}
            }},
            // -Z
            { {0,0,-1}, {
                { h,-h,-h}, { h, h,-h}, {-h, h,-h}, {-h,-h,-h}
            }}
        };

        math::Vector2 uv[4] = {
            {0,1}, {1,1}, {1,0}, {0,0}
        };

        for (const Face& face : faces) {
            uint32_t base = b.vertexCount();

            for (int i = 0; i < 4; ++i) {
                b.addStandardVertex(face.v[i], face.normal, uv[i]);
				//b.addVertex({
                    //.position = face.v[i],
                    //.normal = face.normal,
                    //.uv = uv[i]
                    //});
            }

            b.addQuad(base + 0, base + 1, base + 2, base + 3);
        }

        b.generateTangents();
        b.buildInto(*this);
    }
}