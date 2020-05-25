#ifndef RHEA_PHYSICS_HEIGHTMAP_H
#define RHEA_PHYSICS_HEIGHTMAP_H

#include "Body.h"

namespace Rhea
{
	class Item;

	class PhysicsHeightMap : public PhysicsBody
	{
	public:
		PhysicsHeightMap(Item* item) : 
			PhysicsBody(item)
		{}
	};
}

#endif // !RHEA_PHYSICS_HEIGHTMAP_H
