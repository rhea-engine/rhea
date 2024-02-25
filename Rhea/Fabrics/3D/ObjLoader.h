#ifndef RHEA_OBJLOADER_H
#define RHEA_OBJLOADER_H

#include <string>
#include "../Core/Geometry.h"
#include "Fabrics/3D/Mesh.h"
#include <Math/Vector3.h>
#include <Math/Vector2.h>

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
		bool load(Rhea::Mesh* mesh);

		std::vector<Unit::Export> getReferences();
	
		struct Vertex {
			Math::Vector3 position;
			Math::Vector3 normal;
			Math::Vector3 color;
			Math::Vector2 uv;
			Math::Vector3 tangent;

			bool operator==(const Vertex& other) const {
				return position.equals(other.position)/* && normal.equals(other.normal)*/ && uv.equals(other.uv)/* && color.equals(other.color) */;
			}
		};		
	};
}

#endif // !RHEA_OBJLOADER_H
