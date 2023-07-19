#ifndef RHEA_GEOMETRY_H
#define RHEA_GEOMETRY_H

#include <Unit.h>
#include "../3D/Mesh.h"

namespace Rhea
{
	class Geometry : public Unit
	{
	public:
		Geometry(std::string path_name, std::string type_name) :
			Unit(path_name, type_name)
		{}
		virtual bool get(std::vector<Mesh::vertex>& vertices, std::vector<unsigned long>& indices) = 0;
	};
}

#endif // !RHEA_GEOMETRY_H
