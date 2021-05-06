#ifndef GRAPHICS_SCENE_H
#define GRAPHICS_SCENE_H

#include <Poseidon/Core.h>

#include "../Element.h"
#include "../Scene.h"

#include "GraphicsManager.h"

namespace Rhea
{
	class Item;

	class GraphicsScene : public Element
	{
	public:
		GraphicsScene(Scene* scene) :
			m_scene(scene),
			m_items_valid(false)
		{}

		// get my currently active items
		std::vector<Item*> getItems();
	protected:
		bool m_items_valid;
		std::vector<Item*> m_items;
		Scene* m_scene;
	};

	class GraphicsSceneManager : public GraphicsManager
	{
	public:
		GraphicsSceneManager(GraphicsService* service) :
			GraphicsManager(service),
			m_core(nullptr)
		{}

		void createScene(Poseidon::Core* core, GraphicsScene* elem, Scene* scene);

		// add one item to the scene (creating the corresponding element)
		void addItem(Item* item);

		// handle add item event
		bool onItemAdd(Event* ev, Unit* unit);

		Element* createElement(Poseidon::Core* core, Unit* unit);	

		void onFrame() {}
	
	protected:
		Poseidon::Core* m_core;
	};
}

#endif // !GRAPHICS_SCENE_H
