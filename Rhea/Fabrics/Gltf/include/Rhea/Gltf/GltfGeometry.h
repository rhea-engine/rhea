#ifndef RHEA_GLTFGEOMETRY_H
#define RHEA_GLTFGEOMETRY_H

#include <Rhea/Core/Geometry.h>

namespace rhea
{
	class Application;
	class GltfGeometry : public Geometry
	{
		MECLASS(GltfGeometry, Geometry)
	public:
		GltfGeometry(std::string path_name, std::string type_name);
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);
		bool load(Mesh* mesh) override;
	};
}

#endif // !RHEA_GLTFGEOMETRY_H
