#ifndef RHEA_OBJTRIANGLELOADER_H
#define RHEA_OBJTRIANGLELOADER_H

#include <vector>
#include <Math/Vector3.h>

namespace rhea {
	class Application;
	class Item;
}

struct TriangleSoup {
	std::vector<Math::Vector3> positions;
	std::vector<unsigned long> indices;
};

bool ExtractTrianglesFromScene(rhea::Application* app, rhea::Item* item, TriangleSoup& out);

#endif //  RHEA_OBJTRIANGLELOADER_H
