/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#ifndef RHEA_LINEBATCHMESH_H
#define RHEA_LINEBATCHMESH_H

#include <Rhea/Core/Mesh.h>
#include <Rhea/Resources/VertexBufferColor.h>
#include <Math/Vector3.h>
#include <Math/Vector4.h>
#include <Math/Matrix4.h>

namespace rhea
{
	class LineBatchMesh : public Mesh
	{
		RECLASS(LineBatchMesh, Mesh, "//meworla/linebatchmesh")
	public:
		LineBatchMesh(std::string name, std::string type_name) : Mesh(name, type_name) {}

		void ready() override;

		void addLine(math::Vector3 a, math::Vector3 b, math::Vector4 color);
		void addCube(math::Vector3 min, math::Vector3 max, math::Vector4 color);
		void addCubeCentered(math::Vector3 center, math::Vector3 size, math::Vector4 color);
		void addAxis(const math::Vector3& origin, float size);
		void addAxis(const math::Matrix4& transform, float size);
		void addGrid(const math::Matrix4& transform, float size, int divisions, math::Vector4 color);

		void clearLines();

		void update();

		void markDirty() { m_dirty = true; }

	private:
		std::vector<VertexBufferColor::VertexColor> m_vertices;
		bool m_dirty = false;
	};
}

#endif // !RHEA_LINEBATCHMESH_H