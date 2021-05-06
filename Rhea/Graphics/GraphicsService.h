#ifndef RHEA_GRAPHICS_SERVICE_H
#define RHEA_GRAPHICS_SERVICE_H

#include <memory>
#include <unordered_map>

#include "../Service.h"

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
	public:
		GraphicsService(std::string name, std::string type_name);

		// set the platform service
		void setPlatform(Service* platform_service);

		// set the scene to display
		void setScene(Unit* scene);

		// get the engine
		Poseidon::Core* getEngine() { return m_core.get(); }

		// Service events

		void onInit();

		bool onFrame();
		
		bool onDisplay(Event* ev, Unit* unit);

		// get current dimensions
		int getViewportWidth() { return m_viewport_width; }
		int getViewportHeight() { return m_viewport_height; }

		// get the manager for a certain type
		GraphicsManager* getManager(std::string type_name);

		// factory function
		static Unit* createType(std::string name, std::string type);

	protected:
		std::unique_ptr<Poseidon::Core> m_core;
		Service* m_platform;
		std::unordered_map<std::string, GraphicsManager*> m_managers;

		int m_viewport_width;
		int m_viewport_height;

		// the currently displayed scene
		GraphicsScene* m_g_scene;
	};
}

#endif // !RHEA_GRAPHICS_SERVICE
