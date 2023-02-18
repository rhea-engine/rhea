#ifndef RHEA_TERRAIN_H
#define RHEA_TERRAIN_H

#include <vector>
#include <Poseidon/Material.h>
#include <Psio/Image/Image.h>
#include <Math/Vector3.h>

#include "Fabrics/3D/Item.h"

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

		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// get height in world coords
		float getHeight(float x, float z);

		// get height in map coords
		float getHeightAt(int x, int y);

		// get the heightmap
		Unit* getHeightmap();

		// load terrain
		void ready();

		ImageResource* getTexture() { return m_texture; }		
		Math::Vector3 calculateNormal(int x, int y);

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
		
		static float barryCentric(Math::Vector3 p1, Math::Vector3 p2, Math::Vector3 p3, float x, float z);			
	};
}

#endif // !RHEA_TERRAIN_H
