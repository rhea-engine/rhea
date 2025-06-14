#include <Rhea/Terrain/BulletHeightMap.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Fabrics/Media/ImageResource.h>
#include <Rhea/Terrain/Terrain.h>

#include <Rhea/Graphics/Texture.h>
#include <Poseidon/Texture.h>

#include <bullet/src/btBulletDynamicsCommon.h>
#include <bullet/src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

#include <algorithm>
#undef min
#undef max

namespace Rhea
{
	BulletHeightMap::BulletHeightMap(btDynamicsWorld* world, Item* item) :
		m_width(0),
		m_height(0),
		BulletPhysicsBody(world, item)
	{
		Unit* heightmap = static_cast<Terrain*>(item)->getHeightmap();
		bool flipZ = true;
		m_isStatic = true; // static heightmap
		
		if (heightmap) {

			unsigned char* buf;
			size_t buflen;

			float min_height = 0.0f;
			float max_height = 0.0f;

			m_image = std::make_unique<Poseidon::Image>();
			if (static_cast<Resource*>(heightmap)->getRaw((char**)&buf, &buflen)) {
				m_image->load(buf, buflen);
				m_width = m_image->getWidth();
				m_height = m_image->getHeight();
				if (!flipZ) {
					for (size_t h = 0; h < (size_t)m_height; h++) {
						for (size_t w = 0; w < (size_t)m_width; w++) {
							Poseidon::Image::Color c = m_image->getPixel(w, h);
							unsigned char grey = (unsigned char)(0.299 * c.r + 0.587 * c.g + 0.114 * c.b);
							min_height = std::min((float)grey, min_height);
							max_height = std::max((float)grey, max_height);
							m_map.push_back(grey);
						}
					}
					//printf("minH: %f\tmaxH: %f\n", min_height, max_height);
				}
				else {
					for (int h = m_height - 1; h >= 0; h--) {
						for (size_t w = 0; w < (size_t)m_width; w++) {
							Poseidon::Image::Color c = m_image->getPixel(w, h);
							unsigned char grey = (unsigned char)(0.299 * c.r + 0.587 * c.g + 0.114 * c.b);
							min_height = std::min((float)grey, min_height);
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
		float scale = maxH / 256;
		m_shape = new btHeightfieldTerrainShape(m_width, m_height, m_map.data(), scale, 0, maxH, 1, PHY_UCHAR, false);
		btTransform startTrans = btTransform::getIdentity();
		float offset_y = (minH + maxH) * 0.5f;
		float offset_x = (float)m_width / 2;
		float offset_z = (float)m_height / 2;
		startTrans.setOrigin(btVector3(offset_x, offset_y, offset_z));
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