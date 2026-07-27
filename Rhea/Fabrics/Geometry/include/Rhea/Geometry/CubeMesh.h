/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GEOMETRY_CUBEMESH_H
#define RHEA_GEOMETRY_CUBEMESH_H

#include <Rhea/Geometry/GeometryMesh.h>
#include <algorithm>

namespace rhea
{
	class CubeMesh : public GeometryMesh
	{
		RECLASS(CubeMesh, GeometryMesh, "//meworla/cubemesh")
	public:
		CubeMesh(std::string name, std::string type_name) : GeometryMesh(name, type_name) {}

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;

		void setSize(float size) { m_width = (std::max)(0.001f, size); m_length = (std::max)(0.001f, size); m_height = (std::max)(0.001f, size); markDirty(); }
		float getSize() const { return m_width; }

		void setWidth(float width) { m_width = (std::max)(0.001f, width);  markDirty(); }
		float getWidth() const { return m_width; }

		void setLength(float length) { m_length = (std::max)(0.001f, length);  markDirty(); }
		float getLength() const { return m_length; }

		void setHeight(float height) { m_height = (std::max)(0.001f, height);  markDirty(); }
		float getHeight() const { return m_height; }

		void createMesh() override;

		void setTileSize(float width, float height)
		{
			m_tile_width = (std::max)(0.001f, width);
			m_tile_height = (std::max)(0.001f, height);
			markDirty();
		}

		void setTileWidth(float width)
		{
			m_tile_width = (std::max)(0.001f, width);
			markDirty();
		}

		float getTileWidth() const
		{
			return m_tile_width;
		}

		void setTileHeight(float height)
		{
			m_tile_height = (std::max)(0.001f, height);
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
