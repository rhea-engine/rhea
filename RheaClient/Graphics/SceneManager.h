#ifndef GRAPHICS_SCENE_H
#define GRAPHICS_SCENE_H

#include <Poseidon/Core.h>

#include <Rhea/Component.h>
#include <Rhea/Core/Scene.h>

#include <Rhea/Graphics/GraphicsManager.h>

namespace Rhea
{
	class Item;

	class SceneRenderState : public Component {
		RECLASS(SceneRenderState, Component, "//meworla/scenerenderstate")
	public:
		SceneRenderState(std::string name, std::string type_name) :
			m_scene(nullptr),
			m_items_valid(false),
			Component(name, type_name)
		{
		}
		SceneRenderState(Scene* scene) : 
			m_scene(scene),
			m_items_valid(false),
			Component("scene_render_state", "//meworla/scenerenderstate")
		{}

		void invalidate() { m_items_valid = false; }

		const std::vector<Item*>& getItems(); // cached

	private:
		Scene* m_scene;
		bool m_items_valid;
		std::vector<Item*> m_items;
	};

	class GraphicsSceneManager : public GraphicsManager
	{
	public:
		GraphicsSceneManager(GraphicsService* service) :
			GraphicsManager(service),
			m_core(nullptr)
		{}
		
		// add one item to the scene (creating the corresponding element)
		void addItem(Unit* item);

		// handle add item event
		bool onItemAdd(Event* ev, Unit* unit);

		bool Ensure(Poseidon::Core* core, Unit* unit);	

		void onFrame() {}
	
	protected:
		Poseidon::Core* m_core;
	};
}

#endif // !GRAPHICS_SCENE_H
