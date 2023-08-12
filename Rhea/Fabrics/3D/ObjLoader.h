#ifndef RHEA_OBJLOADER_H
#define RHEA_OBJLOADER_H

#include <string>
#include "../Core/Geometry.h"
#include "Fabrics/3D/Mesh.h"

namespace Rhea
{
	class Application;

	class ObjLoader : public Geometry
	{
		MECLASS(ObjLoader, Geometry)
	public:	
		ObjLoader(std::string path_name, std::string type_name) :
			Geometry(path_name, type_name)
		{}
		bool load(std::string objbuf, std::string mtlbuf, std::vector<Mesh::vertex>& vertices, std::vector<unsigned long>& indices);
		bool get(std::vector<Mesh::vertex>& vertices, std::vector<unsigned long>& indices);

		std::vector<Unit::Export> getReferences();
	};
}

#endif // !RHEA_OBJLOADER_H
