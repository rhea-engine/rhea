#ifndef RHEA_PHYISCS_BULLET_HEIGHTMAP_H
#define RHEA_PHYISCS_BULLET_HEIGHTMAP_H

#include <vector>
#include <memory>
#include <Psio/Image/Image.h>

#include "../PhysicsHeightMap.h"

class btHeightfieldTerrainShape;
class btRigidBody;

namespace Rhea
{
	class Item;

	class BulletHeightMap : public PhysicsHeightMap
	{
	public:
		BulletHeightMap(Item* item);
		~BulletHeightMap();

		btRigidBody* getBody() { return m_body; }

	protected:
		btRigidBody* m_body;
		btHeightfieldTerrainShape* m_shape;
		int m_width;
		int m_height;
		std::vector<unsigned char> m_map;
		std::unique_ptr<Poseidon::Image> m_image;
	};
}

#endif // !RHEA_PHYISCS_BULLET_HEIGHTMAP_H
