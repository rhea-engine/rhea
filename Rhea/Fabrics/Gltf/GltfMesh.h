#ifndef RHEA_GLTF_MESH_H
#define RHEA_GLTF_MESH_H

#include <Rhea/Unit.h>
#include <Rhea/Fabrics/Core/Geometry.h>

namespace Rhea
{
	class GltfMesh : public Geometry
	{
		MECLASS(GltfMesh, Geometry)
	public:
		GltfMesh(std::string name, std::string type_name) : Geometry(name, type_name)
		{}

		virtual ~GltfMesh()
		{}

		std::vector<Unit::Export> getReferences();

		bool load(Rhea::Mesh* mesh);
	};
}
#endif // !RHEA_GLTF_MESH_H
