/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#define NOMINMAX
#include "BulletHeightMap.h"
#include <Rhea/Core/Item.h>
#include <Rhea/Resources/ImageResource.h>
#include <Rhea/Terrain/Terrain.h>
#include <Psio/Log.h>

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

#include <algorithm>

namespace rhea::physics::bullet
{
	BulletHeightMap::BulletHeightMap(btDynamicsWorld* world, rhea::Item* item) :
		m_width(0),
		m_height(0),
		m_shape(nullptr),
		BulletPhysicsBody(world, item)
	{
		rhea::Terrain* terrain = doCast<rhea::Terrain>(item);
		if (!terrain) {
			LOG(LOG_ERROR) << "BulletHeightMap: item is not a Terrain";
			return;
		}
		rhea::Unit* heightmap = terrain->getHeightmap();
		bool flipZ = true;
		m_isStatic = true; // static heightmap
		
		if (heightmap) {

			unsigned char* buf;
			size_t buflen;

			float max_height = 0.0f;

			m_image = std::make_unique<poseidon::Image>();
			if (static_cast<rhea::Resource*>(heightmap)->getRaw((char**)&buf, &buflen)) {
				m_image->load(buf, buflen);
				m_width = m_image->getWidth();
				m_height = m_image->getHeight();
				if (!flipZ) {
					for (size_t h = 0; h < (size_t)m_height; h++) {
						for (size_t w = 0; w < (size_t)m_width; w++) {
							poseidon::Image::Color c = m_image->getPixel(w, h);
							unsigned char grey = (unsigned char)(0.299 * c.r + 0.587 * c.g + 0.114 * c.b);
							max_height = std::max((float)grey, max_height);
							m_map.push_back(grey);
						}
					}
					//printf("minH: %f\tmaxH: %f\n", min_height, max_height);
				}
				else {
					for (int h = m_height - 1; h >= 0; h--) {
						for (size_t w = 0; w < (size_t)m_width; w++) {
							poseidon::Image::Color c = m_image->getPixel(w, h);
							unsigned char grey = (unsigned char)(0.299 * c.r + 0.587 * c.g + 0.114 * c.b);
							max_height = std::max((float)grey, max_height);
							m_map.push_back(grey);
						}
					}
					//printf("minH: %f\tmaxH: %f\n", min_height, max_height);
				}
			}
			else {
				printf("Could not read image data\n");
				return;
			}

			printf("Texture: %d:%d Channels:%d\n\n", m_width, m_height, m_image->getChannels());
		}
		else {
			printf("no heightmap found\n");
		}

		float minH = 0;
		float maxH = item->getValue("height");
		if (!maxH)
			maxH = 255;
		float scale = maxH / 255;
		if (m_width > 0 && m_height > 0 && m_map.size() > 0) {
			m_shape = new btHeightfieldTerrainShape(m_width, m_height, m_map.data(), scale, 0, maxH, 1, PHY_UCHAR, false);
			if (!m_shape) {
				LOG(LOG_ERROR) << "BulletHeightMap: could not create shape";
				return;
			}
			// shapes are owned by the rigid body, so we need to keep track of them to delete them later
			m_shapes.push_back(m_shape);
		}
		else {
			LOG(LOG_ERROR) << "BulletHeightMap: invalid heightmap data for item " << item->getPath();
			return;
		}
		float sizeX = static_cast<float>(terrain->getTerrainWidth());
		float sizeZ = static_cast<float>(terrain->getTerrainLength());

		btTransform startTrans = btTransform::getIdentity();
		float offset_y = (minH + maxH) * 0.5f;
		float offset_x = sizeX * 0.5f;
		float offset_z = sizeZ * 0.5f;
		// add terrain position
		auto worldTransform = item->getTransform()->getWorldPosition();
		math::Vector3 terrainPosition =	worldTransform.getTranslation();

		startTrans.setOrigin(btVector3(
			terrainPosition.x + offset_x,
			terrainPosition.y + offset_y,
			terrainPosition.z + offset_z
		));

		float scaleX = sizeX / static_cast<float>(m_width - 1);
		float scaleZ = sizeZ / static_cast<float>(m_height - 1);

		m_shape->setLocalScaling(
			btVector3(scaleX, 1.0f, scaleZ)
		);

//		startTrans.setOrigin(btVector3(offset_x, offset_y, offset_z));
		btMotionState* ms = new btDefaultMotionState(startTrans);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(0, ms, m_shape, btVector3(0, 0, 0));
		m_body = new btRigidBody(rbInfo);
		m_body->setUserPointer((void*)item);

		/*
		m_shape = new btHeightfieldTerrainShape(s_gridSize, s_gridSize,
			m_rawHeightfieldData,
			s_gridHeightScale,
			m_minHeight, m_maxHeight,
			m_upAxis, m_type, flipQuadEdges);

		
		
		
		float minH, maxH; //this is the minimum and maximum value from my data 
		btHeightfieldTerrainShape * heightfieldShape = new btHeightfieldTerrainShape(width,height,data,1, minH, maxH,1, PHY_FLOAT, false);
		float offset = (minH + maxH) * 0.5f; //here we go calculating the offset to compensate for bullet
		btTransform startTrans = btTransform::getIdentity();
		startTrans.setOrigin(btVector3(0,offset,0); //this should put things back where you'd expect
													//and so on... 
		btMotionState* ms = new btDefaultMotionState(startTrans);
		btRigidBody::btRigidBodyConstructionInfo info(0,ms,heightfieldShape,btVector3(0,0,0));
		*/

	}

	BulletHeightMap::~BulletHeightMap()
	{
	}
}