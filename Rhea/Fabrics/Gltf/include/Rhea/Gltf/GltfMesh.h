/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GLTF_MESH_H
#define RHEA_GLTF_MESH_H

#include <Rhea/Unit.h>
#include <Rhea/Core/Geometry.h>

namespace rhea
{
	class GltfMesh : public Geometry
	{
		MECLASS(GltfMesh, Geometry)
	public:
		GltfMesh(std::string name, std::string type_name) : Geometry(name, type_name)
		{}

		virtual ~GltfMesh()
		{}

		std::vector<Unit::ResourceReference> getReferences();

		bool load(rhea::Mesh* mesh);
	};
}
#endif // !RHEA_GLTF_MESH_H
