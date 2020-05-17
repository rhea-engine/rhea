#ifndef RHEA_SCENE_H
#define RHEA_SCENE_H

#include <vector>

#include "Resource.h"

namespace Rhea
{
	class Item;

	// a scene is a holder for various corresponding scene objects
	class Scene : public Resource
	{
	public:
		Scene(std::string name) :
			Resource(name, "//meworla/scene")
		{}

		// add an item to the scene
		void addItem(Item* item);

		// get the items
		std::vector<Item*> getItems() { return m_items; }

		void load(ReadyHandler handler);
	protected:
		std::vector<Item*> m_items;		
	};
}

#endif // !RHEA_SCENE_H
