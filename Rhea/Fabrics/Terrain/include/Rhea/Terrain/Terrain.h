/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_TERRAIN_H
#define RHEA_TERRAIN_H

#include <vector>
#include <Rhea/Core/Material.h>
#include <Psio/Image/Image.h>
#include <Math/Vector3.h>

#include <Rhea/Core/Item.h>

namespace rhea
{
	class ImageResource;
	class Unit;

	class Terrain : public Item
	{
		MECLASS(Terrain, Item)
	public:
		Terrain(std::string name, std::string type_name) :			
			m_width(0),
			m_length(0),
			m_max_height(256.0f),
			m_min_height(0.0f),
			m_heights(nullptr),
			m_material(nullptr),
			Item(name, type_name)
		{}
		~Terrain();

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

		int getWidth() { return m_width; }
		int getLength() { return m_length; }

		void setTerrainSize(int w, int h) { m_sizeX = w, m_sizeY = h; }
		int getTerrainWidth() { return m_sizeX; }
		int getTerrainLength() { return m_sizeY; }
		
		math::Vector3 calculateNormal(int x, int y);

		static void initializeType(simpleparser::Type* type);	

	protected:
		// size of heightmap image
		int m_width;
		int m_length;
		// size of terrain
		int m_sizeX = -1;
		int m_sizeY = -1;
		float m_min_height;
		float m_max_height;
		std::unique_ptr<poseidon::Image> m_heightmap;
		float* m_heights;
		Material* m_material;

		Ref<ImageResource> m_heightmap_resource;
		Ref<ImageResource> m_blendmap_resource;
		Ref<ImageResource> m_texture1_resource;
		Ref<ImageResource> m_texture2_resource;
		Ref<ImageResource> m_texture3_resource;
		Ref<ImageResource> m_texture4_resource;		
		Ref<Shader> m_shader_resource;

		static float barryCentric(math::Vector3 p1, math::Vector3 p2, math::Vector3 p3, float x, float z);			
	};
}

#endif // !RHEA_TERRAIN_H
