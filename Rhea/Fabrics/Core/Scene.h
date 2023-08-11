#ifndef RHEA_SCENE_H
#define RHEA_SCENE_H

#include <vector>

#include "Rhea/Resources/Resource.h"

namespace Rhea
{
	class Item;

	// a scene is a holder for various corresponding scene objects
	class Scene : public Unit
	{
	public:
		Scene(std::string name) :
			m_is_active(false),
			Unit(name, "//meworla/scene")
		{}

		// implement onParented
		void onParented();

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

		// handle scene show/hide
		void show();
		void hide();

		// handle scene active state
		void activate();
		void deactive();

		// factory function
		static Unit* createType(std::string name, std::string type);

	protected:
		std::vector<Item*> m_items;		
		bool m_is_active;
	};
}

#endif // !RHEA_SCENE_H
