#ifndef RHEA_TERRAIN_H
#define RHEA_TERRAIN_H

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
			Item(name, type_name)
		{}

		// load terrain
		void load(ReadyHandler handler);

		ImageResource* getTexture() { return m_texture; }

		static Unit* createType(std::string name, std::string type);

	protected:
		ImageResource* m_texture;
	};
}

#endif // !RHEA_TERRAIN_H
