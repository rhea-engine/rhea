/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GEOMETRY_CUBEMESH_H
#define RHEA_GEOMETRY_CUBEMESH_H

#include <Rhea/Core/Mesh.h>

namespace rhea
{
	class CubeMesh : public Mesh
	{
		RECLASS(CubeMesh, Mesh, "//meworla/cubemesh")
	public:
		CubeMesh(std::string name, std::string type_name) : Mesh(name, type_name) {}

		void createMesh();

	protected:
		float m_size = 1.0f;
	};
}
#endif // !RHEA_GEOMETRY_CUBEMESH_H
