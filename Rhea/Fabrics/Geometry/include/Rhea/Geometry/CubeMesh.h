/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GEOMETRY_CUBEMESH_H
#define RHEA_GEOMETRY_CUBEMESH_H

#include <Rhea/Geometry/GeometryMesh.h>

namespace rhea
{
	class CubeMesh : public GeometryMesh
	{
		RECLASS(CubeMesh, GeometryMesh, "//meworla/cubemesh")
	public:
		CubeMesh(std::string name, std::string type_name) : GeometryMesh(name, type_name) {}

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		void setSize(float size) { m_width = size; m_length = size; m_height = size; markDirty(); }
		float getSize() const { return m_width; }

		void setWidth(float width) { m_width = width;  markDirty(); }
		float getWidth() const { return m_width; }

		void setLength(float length) { m_length = length;  markDirty(); }
		float getLength() const { return m_length; }

		void setHeight(float height) { m_height = height;  markDirty(); }
		float getHeight() const { return m_height; }

		void createMesh() override;

		void setTileSize(float width, float height)
		{
			m_tile_width = width;
			m_tile_height = height;
			markDirty();
		}

		void setTileWidth(float width)
		{
			m_tile_width = width;
			markDirty();
		}

		float getTileWidth() const
		{
			return m_tile_width;
		}

		void setTileHeight(float height)
		{
			m_tile_height = height;
			markDirty();
		}

		float getTileHeight() const
		{
			return m_tile_height;
		}

	protected:
		float m_width = 1.0f;
		float m_length = 1.0f;
		float m_height = 1.0f;
		float m_tile_width = 2.0f;
		float m_tile_height = 2.0f;
	};
}
#endif // !RHEA_GEOMETRY_CUBEMESH_H
