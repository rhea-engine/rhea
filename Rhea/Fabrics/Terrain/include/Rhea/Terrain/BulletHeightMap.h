#ifndef RHEA_PHYISCS_BULLET_HEIGHTMAP_H
#define RHEA_PHYISCS_BULLET_HEIGHTMAP_H

#include <vector>
#include <memory>
#include <Psio/Image/Image.h>

#include <Rhea/Physics/Bullet/BtBody.h>

class btHeightfieldTerrainShape;
class btRigidBody;

namespace Rhea
{
	class Item;

	class BulletHeightMap : public BulletPhysicsBody
	{
	public:
		BulletHeightMap(btDynamicsWorld* world, Item* item);
		~BulletHeightMap();

		bool init() override { return true; }

	protected:
		btHeightfieldTerrainShape* m_shape;
		int m_width;
		int m_height;
		std::vector<unsigned char> m_map;
		std::unique_ptr<Poseidon::Image> m_image;
	};
}

#endif // !RHEA_PHYISCS_BULLET_HEIGHTMAP_H
