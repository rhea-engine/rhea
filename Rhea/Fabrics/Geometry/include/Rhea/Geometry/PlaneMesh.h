/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GEOMETRY_PLANEMESH_H
#define RHEA_GEOMETRY_PLANEMESH_H

#include <Rhea/Core/Mesh.h>
#include <Math/Vector2.h>

namespace rhea
{
	class PlaneMesh : public Mesh
	{
		RECLASS(PlaneMesh, Mesh, "//meworla/planemesh")
	public:
		PlaneMesh(std::string name, std::string type_name) : Mesh(name, type_name) {}

		void setSize(float width, float height) { m_width = width; m_height = height; markDirty(); }
		void setWidth(float width) { m_width = width; markDirty(); }
		void setHeight(float height) { m_height = height; markDirty(); }

		void setSegments(uint32_t x, uint32_t y) { m_segmentsX = x; m_segmentsY = y; markDirty(); }
		void setSegmentsX(uint32_t x) { m_segmentsX = x; markDirty(); }
		void setSegmentsY(uint32_t y) { m_segmentsY = y; markDirty(); }

		void setCentered(bool centered) { m_centered = centered; markDirty(); }
		void setUVScale(float u, float v) { m_uvScale = { u, v }; markDirty(); }

		float width() const { return m_width; }
		float height() const { return m_height; }
		uint32_t segmentsX() const { return m_segmentsX; }
		uint32_t segmentsY() const { return m_segmentsY; }
		bool centered() const { return m_centered; }

		void rebuild();

		void markDirty() { m_dirty = true; }

	protected:
		float m_width = 1.0f;
		float m_height = 1.0f;
		uint32_t m_segmentsX = 1;
		uint32_t m_segmentsY = 1;
		bool m_centered = true;  // centered at origin or starting at (0,0)
		math::Vector2 m_uvScale = { 1.0f, 1.0f }; 
		bool m_dirty = true;
	};
}
#endif // !RHEA_GEOMETRY_PLANEMESH_H
