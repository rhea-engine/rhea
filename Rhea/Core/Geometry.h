#ifndef RHEA_GEOMETRY_H
#define RHEA_GEOMETRY_H

#include <Rhea/Unit.h>
#include "../Fabrics/3D/Mesh.h"
#include <Rhea/Resources/VertexAttribute.h>
#include <Rhea/Resources/VertexBuffer.h>

namespace Rhea
{
	class Geometry : public Unit
	{
		MECLASS_VIRTUAL(Geometry, Unit)
	public:
		Geometry(std::string path_name, std::string type_name) :
			Unit(path_name, type_name)
		{}		
		virtual bool load(Mesh* mesh) = 0;
	};
}

#endif // !RHEA_GEOMETRY_H
