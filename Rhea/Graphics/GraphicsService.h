#ifndef RHEA_GRAPHICS_SERVICE_H
#define RHEA_GRAPHICS_SERVICE_H

#include <memory>
#include <unordered_map>

#include "../Service.h"
#include <Rhea/Fabrics/Core/Scene.h>

namespace Poseidon
{
	class Core;
}

namespace Rhea
{
	class Unit;
	class Element;
	class Event;
	class GraphicsManager;
	class GraphicsScene;

	class GraphicsService : public Service
	{
		MECLASS(GraphicsService, Service)
	public:
		GraphicsService(std::string name, std::string type_name);
		~GraphicsService();

		// set the platform service
		void setPlatform(Service* platform_service);

		// prepare a scene to display
		void onSceneActivate(Scene* scene);
		void onSceneDeactivate(Scene* scene);

		// show the targeted scene
		void showScene(Scene* scene);
		void hideScene(Scene* scene);

		// add item to render context
		void addItem(std::string render_context, Item* item);

		// get the engine
		Poseidon::Core* getEngine() { return m_core.get(); }

		// Service events

		void onInit();

		bool onFrameFinal(float delta) override;
		
		bool onInput(Event* ev, Unit* unit);
		bool onAddItem(Event* ev, Unit* unit);

		// get current dimensions
		int getViewportWidth() { return m_viewport_width; }
		int getViewportHeight() { return m_viewport_height; }

		// request fullscreen
		void ToggleFullscreen();

		// get the manager for a certain type
		GraphicsManager* getManager(SimpleParser::Type* type);

		template<typename T>
		GraphicsManager* getManager() {
			auto it = m_managers.find(T::staticType());
			if (it == m_managers.end())
				return nullptr;
			else
				return it->second.get();
		}

		// create element for the unit
		bool addElement(Unit* unit);

	protected:
		std::unique_ptr<Poseidon::Core> m_core;
		Service* m_platform;
		std::unordered_map<SimpleParser::Type*, std::unique_ptr<GraphicsManager>> m_managers;

		template<typename T>
		void addManager(std::unique_ptr<GraphicsManager> mgr) {
			m_managers[T::staticType()] = std::move(mgr);
		}

		int m_viewport_width;
		int m_viewport_height;

		// the activated scenes
		std::vector<Scene*> m_scenes;

		// the current mouse pos
		int m_mx;
		int m_my;

		// fullscreen mode
		bool m_fullscreen;

		Unit* pick();
	};
}

#endif // !RHEA_GRAPHICS_SERVICE
