#ifndef RHEA_SCENE_H
#define RHEA_SCENE_H

#include <vector>

#include "Rhea/Resource.h"

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

		// remove item from scene
		void removeItem(Item* item);

		// get the items
		std::vector<Item*> getItems() { return m_items; }

		// ready event handler
		void ready();

		// handle new items in scene
		bool onSceneEnter(Event* ev, Unit* unit);

		// factory function
		static Unit* createType(std::string name, std::string type);

	protected:
		std::vector<Item*> m_items;		
	};
}

#endif // !RHEA_SCENE_H
