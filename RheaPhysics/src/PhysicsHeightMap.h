#ifndef RHEA_PHYSICS_HEIGHTMAP_H
#define RHEA_PHYSICS_HEIGHTMAP_H

#include "NullBody.h"

namespace rhea
{
	class Item;
}

namespace rhea::physics
{
	class PhysicsHeightMap : public NullPhysicsBody
	{
	public:
		PhysicsHeightMap(rhea::Item* item) :
			NullPhysicsBody(item)
		{
			m_isStatic = true;
		}
	};
}

#endif // !RHEA_PHYSICS_HEIGHTMAP_H
