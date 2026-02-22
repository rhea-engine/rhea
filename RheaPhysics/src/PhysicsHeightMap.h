#ifndef RHEA_PHYSICS_HEIGHTMAP_H
#define RHEA_PHYSICS_HEIGHTMAP_H

#include "NullBody.h"

namespace Rhea
{
	class Item;

	class PhysicsHeightMap : public NullPhysicsBody
	{
	public:
		PhysicsHeightMap(Item* item) :
			NullPhysicsBody(item)
		{
			m_isStatic = true;
		}
	};
}

#endif // !RHEA_PHYSICS_HEIGHTMAP_H
