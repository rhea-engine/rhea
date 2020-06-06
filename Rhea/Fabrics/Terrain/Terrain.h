#ifndef RHEA_TERRAIN_H
#define RHEA_TERRAIN_H

#include <vector>
#include <Poseidon/Material.h>
#include <Poseidon/Image.h>
#include <Poseidon/Math/Vector3.h>

#include "Item.h"

namespace Rhea
{
	class ImageResource;
	class Unit;

	class Terrain : public Item
	{
	public:
		Terrain(std::string name, std::string type_name) :
			m_texture(nullptr),
			m_width(0),
			m_length(0),
			m_max_height(256.0f),
			m_min_height(0.0f),
			m_heightmap(nullptr),
			m_heights(nullptr),
			Item(name, type_name)
		{}

		// get height in world coords
		float getHeight(float x, float z);

		// get height in map coords
		float getHeightAt(int x, int y);

		// load terrain
		void load(ReadyHandler handler);

		ImageResource* getTexture() { return m_texture; }		
		Poseidon::Vector3 calculateNormal(int x, int y);

		// factory
		static Unit* createType(std::string name, std::string type);

	protected:
		ImageResource* m_texture;		
		int m_width;
		int m_length;
		float m_min_height;
		float m_max_height;
		Poseidon::Image* m_heightmap;
		float* m_heights;
		
		static float barryCentric(Poseidon::Vector3 p1, Poseidon::Vector3 p2, Poseidon::Vector3 p3, float x, float z);			
	};
}

#endif // !RHEA_TERRAIN_H
