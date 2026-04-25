#ifndef GRAPHICS_SCENE_H
#define GRAPHICS_SCENE_H

#include <Poseidon/Core.h>

#include <Rhea/Component.h>
#include <Rhea/Core/Scene.h>

#include <RheaClient/Graphics/GraphicsManager.h>

namespace rhea {
	class Unit;
	class Item;
}

namespace rhea::client {

	class SceneRenderState : public rhea::Component {
		RECLASS(SceneRenderState, rhea::Component, "//meworla/scenerenderstate")
	public:
		SceneRenderState(std::string name, std::string type_name) :
			m_scene(nullptr),
			m_items_valid(false),
			rhea::Component(name, type_name)
		{
		}
		SceneRenderState(rhea::Scene* scene) : 
			m_scene(scene),
			m_items_valid(false),
			rhea::Component("scene_render_state", "//meworla/scenerenderstate")
		{}

		void invalidate() { m_items_valid = false; }

		const std::vector<rhea::Item* >& getItems(); // cached

	private:
		rhea::Scene* m_scene;
		bool m_items_valid;
		std::vector<rhea::Item* > m_items;
	};

	class GraphicsSceneManager : public GraphicsManager
	{
	public:
		GraphicsSceneManager(GraphicsService* service) :
			GraphicsManager(service),
			m_core(nullptr)
		{}
		
		// add one item to the scene (creating the corresponding rhea::element)
		void addItem(rhea::Unit* item);
		
		// remove one item from the scene (removing from render context)
		void removeItem(rhea::Unit* item);

		// handle add item event
		bool onItemAdd(rhea::Event* ev, rhea::Unit* unit);
		// handle remove item event
		bool onItemRemove(rhea::Event* ev, rhea::Unit* unit);

		bool Ensure(poseidon::Core* core, rhea::Unit* unit);	

		void onFrame(float delta) override {}
	
	protected:
		poseidon::Core* m_core;
	};
}

#endif // !GRAPHICS_SCENE_H
