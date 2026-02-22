#ifndef RHEA_OBJTRIANGLELOADER_H
#define RHEA_OBJTRIANGLELOADER_H

#include <vector>
#include <Components/Math/Vector3.h>

namespace Rhea {
	class Application;
	class Item;
}

struct TriangleSoup {
	std::vector<Math::Vector3> positions;
	std::vector<unsigned long> indices;
};

bool ExtractTrianglesFromScene(Rhea::Application* app, Rhea::Item* item, TriangleSoup& out);

#endif //  RHEA_OBJTRIANGLELOADER_H
