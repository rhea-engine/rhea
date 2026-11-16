#ifndef RHEA_OBJGEOMETRY_H
#define RHEA_OBJGEOMETRY_H

#include <Rhea/Core/Geometry.h>

namespace rhea
{
	class ObjGeometry : public Geometry
	{
		MECLASS(ObjGeometry, Geometry)
	public:
		ObjGeometry(std::string path_name, std::string type_name);
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);
		bool load(Mesh* mesh) override;
	};
}
#endif // !RHEA_OBJGEOMETRY_H