#ifndef RHEA_PHYSICS_COLLIDERCOMPONENT_H
#define RHEA_PHYSICS_COLLIDERCOMPONENT_H

#include "../Component.h"
#include <Components/Math/Vector3.h>

namespace Rhea {

	class ColliderComponent : public Component {
		RECLASS(ColliderComponent, Component, "//meworla/collider")
	public:
		ColliderComponent(std::string name, std::string type_name) :			
			Component(name, type_name)
		{
		}

		// What collider to create
		std::string kind;   // "trimesh", "box", "sphere", "capsule"

		// For trimesh: where triangles come from
		std::string source; // "//local/cube.obj" OR "cube2/cube_obj" OR "cube2/mesh"

		// Common params (used depending on kind)
		float radius = 0.0f;      // sphere/capsule
		float height = 0.0f;      // capsule
		Math::Vector3 halfExtents = Math::Vector3(0, 0, 0); // box

		// Optional transform tweak
		float offsetY = 0.0f;
	};
}
#endif // !RHEA_PHYSICS_COLLIDERCOMPONENT_H
