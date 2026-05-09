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
		void setSize(float size) { m_size = size; markDirty(); }
		float getSize() const { return m_size; }

		void createMesh() override;

	protected:
		float m_size = 1.0f;
	};
}
#endif // !RHEA_GEOMETRY_CUBEMESH_H
