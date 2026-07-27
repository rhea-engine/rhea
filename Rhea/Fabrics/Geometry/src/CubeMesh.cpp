/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#define NOMINMAX
#include <Rhea/Geometry/CubeMesh.h>
#include <Rhea/Geometry/MeshBuilder.h>
#include <algorithm>

namespace rhea
{
    void CubeMesh::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
    {
        GeometryMesh::setup(app, attributes, values);

        if (values.find("width") != values.end())
            m_width = values["width"].toFloat();
        if (values.find("height") != values.end())
            m_height = values["height"].toFloat();
        if (values.find("length") != values.end())
            m_length = values["length"].toFloat();
        if (values.find("size") != values.end() && m_values["size"].size() == 3) {
            Variant size = m_values["size"];
            m_width = size[0];
            m_length = size[2];
            m_height = size[1];
        }
        if (values.find("tileSize") != values.end()) {
			if (values["tileSize"].size() == 2) {
				Variant tileSize = values["tileSize"];
				m_tile_width = tileSize[0].toFloat();
				m_tile_height = tileSize[1].toFloat();
			}
            else {
                m_tile_width = values["tileSize"].toFloat();
                m_tile_height = values["tileSize"].toFloat();
            }
        }
    }

    static void makeUV(
        math::Vector2(&uv)[4],
        float repeatU,
        float repeatV,
        bool rotated = false)
    {
        if (!rotated) {
            uv[0] = { 0.0f,    repeatV };
            uv[1] = { repeatU, repeatV };
            uv[2] = { repeatU, 0.0f };
            uv[3] = { 0.0f,    0.0f };
        }
        else {
            uv[0] = { 0.0f,    repeatV };
            uv[1] = { 0.0f,    0.0f };
            uv[2] = { repeatU, 0.0f };
            uv[3] = { repeatU, repeatV };
        }
    }

	void CubeMesh::createMesh()
	{
        MeshBuilder b;

        const float halfX = m_width * 0.5f;
        const float halfY = m_height * 0.5f;
        const float halfZ = m_length * 0.5f;

        const float safeTileWidth = std::max(m_tile_width, 0.0001f);

        const float safeTileHeight = std::max(m_tile_height, 0.0001f);


        const float repeatX = m_width * safeTileWidth;
        const float repeatY = m_height * safeTileHeight;
        const float repeatZ = m_length * safeTileWidth;

        struct Face {
            math::Vector3 normal;
            math::Vector3 vertices[4];
            math::Vector2 uv[4];
        };

        const Face faces[] = {
            // +X
            { { 1, 0, 0 }, {
                { halfX, -halfY, -halfZ },
                { halfX, -halfY,  halfZ },
                { halfX,  halfY,  halfZ },
                { halfX,  halfY, -halfZ }
            },
            {
                { 0,       repeatY },
                { repeatZ, repeatY },
                { repeatZ, 0       },
                { 0,       0       }
            }},

            // -X
            { { -1, 0, 0 }, {
                { -halfX, -halfY,  halfZ },
                { -halfX, -halfY, -halfZ },
                { -halfX,  halfY, -halfZ },
                { -halfX,  halfY,  halfZ }
            },
            {
                { 0,       repeatY },
                { repeatZ, repeatY },
                { repeatZ, 0       },
                { 0,       0       }
            }},

            // +Y
            { { 0, 1, 0 }, {
                { -halfX, halfY, -halfZ },
                {  halfX, halfY, -halfZ },
                {  halfX, halfY,  halfZ },
                { -halfX, halfY,  halfZ }
            },
            {
                { 0,       repeatZ },
                { repeatX, repeatZ },
                { repeatX, 0       },
                { 0,       0       }
            }},

            // -Y
            { { 0, -1, 0 }, {
                { -halfX, -halfY,  halfZ },
                {  halfX, -halfY,  halfZ },
                {  halfX, -halfY, -halfZ },
                { -halfX, -halfY, -halfZ }
            },
            {
                { 0,       repeatZ },
                { repeatX, repeatZ },
                { repeatX, 0       },
                { 0,       0       }
            }},

            // +Z
            { { 0, 0, 1 }, {
                { -halfX, -halfY, halfZ },
                { -halfX,  halfY, halfZ },
                {  halfX,  halfY, halfZ },
                {  halfX, -halfY, halfZ }
            },
            {
                { 0,       repeatY },
                { 0,       0       },
                { repeatX, 0       },
                { repeatX, repeatY }
            }},

            // -Z
            { { 0, 0, -1 }, {
                {  halfX, -halfY, -halfZ },
                {  halfX,  halfY, -halfZ },
                { -halfX,  halfY, -halfZ },
                { -halfX, -halfY, -halfZ }
            },
            {
                { 0,       repeatY },
                { 0,       0       },
                { repeatX, 0       },
                { repeatX, repeatY }
            }}
        };

        for (const Face& face : faces) {
            const uint32_t base = b.vertexCount();

            for (int i = 0; i < 4; ++i)
                b.addStandardVertex(
                    face.vertices[i],
                    face.normal,
                    face.uv[i]
                );

            b.addQuad(base, base + 1, base + 2, base + 3);
        }

        b.generateTangents();
        b.buildInto(*this);
    }
}