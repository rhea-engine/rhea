#ifndef GRAPHICS_SCENE_H
#define GRAPHICS_SCENE_H

#include <Poseidon/Core.h>

#include <Rhea/Component.h>
#include <Rhea/Core/Scene.h>

#include <Rhea/Graphics/GraphicsManager.h>

namespace Rhea {
	class Unit;
	class Item;
}

namespace rhea::client {

	class SceneRenderState : public Rhea::Component {
		RECLASS(SceneRenderState, Rhea::Component, "//meworla/scenerenderstate")
	public:
		SceneRenderState(std::string name, std::string type_name) :
			m_scene(nullptr),
			m_items_valid(false),
			Rhea::Component(name, type_name)
		{
		}
		SceneRenderState(Rhea::Scene* scene) : 
			m_scene(scene),
			m_items_valid(false),
			Rhea::Component("scene_render_state", "//meworla/scenerenderstate")
		{}

		void invalidate() { m_items_valid = false; }

		const std::vector<Rhea::Item* >& getItems(); // cached

	private:
		Rhea::Scene* m_scene;
		bool m_items_valid;
		std::vector<Rhea::Item* > m_items;
	};

	class GraphicsSceneManager : public GraphicsManager
	{
	public:
		GraphicsSceneManager(GraphicsService* service) :
			GraphicsManager(service),
			m_core(nullptr)
		{}
		
		// add one item to the scene (creating the corresponding Rhea::element)
		void addItem(Rhea::Unit* item);

		// handle add item event
		bool onItemAdd(Rhea::Event* ev, Rhea::Unit* unit);

		bool Ensure(Poseidon::Core* core, Rhea::Unit* unit);	

		void onFrame() {}
	
	protected:
		Poseidon::Core* m_core;
	};
}

#endif // !GRAPHICS_SCENE_H
