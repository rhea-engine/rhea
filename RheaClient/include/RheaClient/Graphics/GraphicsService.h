#ifndef RHEA_GRAPHICS_SERVICE_H
#define RHEA_GRAPHICS_SERVICE_H

#include <memory>
#include <unordered_map>

#include <Rhea/Graphics/IGraphicsService.h>
#include <Rhea/Core/Scene.h>
#include "MeshGpuCache.h"
#include "TextureGpuCache.h"
#include "MaterialGpuCache.h"

namespace poseidon
{
	class Core;
}

namespace rhea
{
	class Unit;
	class Element;
	class Event;	
}

namespace rhea::client {
	class GraphicsManager;
	class GraphicsScene;

	class GraphicsService : public rhea::IGraphicsService
	{
		MECLASS(GraphicsService, rhea::IGraphicsService)
	public:
		GraphicsService(std::string name, std::string type_name);
		~GraphicsService();

		// set the platform service
		void setPlatform(rhea::Service* platform_service);

		// prepare a scene to display
		void onSceneActivate(rhea::Scene* scene);
		void onSceneDeactivate(rhea::Scene* scene);

		// show the targeted scene
		void showScene(rhea::Scene* scene);
		void hideScene(rhea::Scene* scene);

		// add item to render context
		void addItem(std::string render_context, rhea::Item* item);

		// get the engine
		poseidon::Core* getEngine() { return m_core.get(); }

		// Service events

		void onInit();

		bool onFrameFinal(float delta) override;
		
		bool onInput(rhea::Event* ev, rhea::Unit* unit);
		bool onAddItem(rhea::Event* ev, rhea::Unit* unit);
		
		// request fullscreen
		void ToggleFullscreen();

		// get the manager for a certain type
		GraphicsManager* getManager(simpleparser::Type* type);

		template<typename T>
		GraphicsManager* getManager() {
			auto it = m_managers.find(T::staticType());
			if (it == m_managers.end())
				return nullptr;
			else
				return it->second.get();
		}

		// create element for the unit
		bool addElement(rhea::Unit* unit);

		MeshGpuCache& meshCache() { return m_meshCache; }
		TextureGpuCache& textureCache() { return m_textureCache; }
		MaterialGpuCache& materialCache() { return m_materialCache; }

	protected:
		std::unique_ptr<poseidon::Core> m_core;
		poseidon::Window* m_main_window;
		rhea::Service* m_platform;
		std::unordered_map<simpleparser::Type*, std::unique_ptr<GraphicsManager>> m_managers;

		template<typename T>
		void addManager(std::unique_ptr<GraphicsManager> mgr) {
			m_managers[T::staticType()] = std::move(mgr);
		}
		
		// the activated scenes
		std::vector<rhea::Scene* > m_scenes;

		// the current mouse pos
		int m_mx;
		int m_my;

		// fullscreen mode
		bool m_fullscreen;

		rhea::Unit* pick();

		MeshGpuCache m_meshCache; 
		TextureGpuCache m_textureCache;
		MaterialGpuCache m_materialCache;
	};
}

#endif // !RHEA_GRAPHICS_SERVICE
