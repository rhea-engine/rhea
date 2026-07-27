/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GEOMETRY_CYLINDERMESH_H
#define RHEA_GEOMETRY_CYLINDERMESH_H

#include <Rhea/Geometry/GeometryMesh.h>
#include <algorithm>

namespace rhea
{
	class CylinderMesh : public GeometryMesh
	{
		RECLASS(CylinderMesh, GeometryMesh, "//meworla/cylindermesh")
	public:
		CylinderMesh(std::string name, std::string type_name) : GeometryMesh(name, type_name) {}
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;

		void setRadius(float radius) { m_radius = (std::max)(0.001f, radius); markDirty(); }
		void setHeight(float height) { m_height = (std::max)(0.001f, height); markDirty(); }
		void setSegments(uint32_t segments) { m_segments = (std::max)(3u, segments); markDirty(); }
		void setCaps(bool caps) { m_caps = caps; markDirty(); }

		void createMesh() override;

	private:
		float m_radius = 0.5f;
		float m_height = 1.0f;
		uint32_t m_segments = 32;
		bool m_caps = true;
	};
}
#endif // !RHEA_GEOMETRY_CYLINDERMESH_H
