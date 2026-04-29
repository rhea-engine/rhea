/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_SCENE_H
#define RHEA_SCENE_H

#include <vector>

#include <Rhea/Unit.h>

namespace rhea
{
	class Item;

	// a scene is a holder for various corresponding scene objects
	class Scene : public Unit
	{
		MECLASS(Scene, Unit)
	public:
		Scene(std::string name) :			
			Scene(name, "//meworla/scene")
		{}
		Scene(std::string name, std::string type_name) :
			m_is_active(false),
			Unit(name, type_name)
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

		// handle new items in scene
		bool onItemAttached(Event* ev, Unit* unit);
		// handle remove of items in scene
		bool onItemDetached(Event* ev, Unit* unit);

		// handle scene show/hide
		void show();
		void hide();

		// handle scene active state
		void activate();
		void deactive();
		
	protected:
		std::vector<Item*> m_items;		
		bool m_is_active;
	};
}

#endif // !RHEA_SCENE_H
