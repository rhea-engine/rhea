/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_PHYISCS_BULLET_HEIGHTMAP_H
#define RHEA_PHYISCS_BULLET_HEIGHTMAP_H

#include <vector>
#include <memory>
#include <Psio/Image/Image.h>

#include "BtBody.h"

class btHeightfieldTerrainShape;
class btRigidBody;

namespace rhea
{
	class Item;
	struct PhysicsBodyDesc;
}

namespace rhea::physics::bullet
{
	class BulletHeightMap : public BulletPhysicsBody
	{
	public:
		BulletHeightMap(btDynamicsWorld* world, rhea::Item* item, const rhea::PhysicsBodyDesc& desc);
		~BulletHeightMap();

		bool init() override { return true; }

	protected:
		btHeightfieldTerrainShape* m_shape;
		int m_width;
		int m_height;
		std::vector<unsigned char> m_map;
		std::unique_ptr<poseidon::Image> m_image;
	};
}

#endif // !RHEA_PHYISCS_BULLET_HEIGHTMAP_H
