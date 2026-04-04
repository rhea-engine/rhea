#ifndef RHEA_GRAPHICS_SERVICE_H
#define RHEA_GRAPHICS_SERVICE_H

#include <memory>
#include <unordered_map>

#include <Rhea/Graphics/IGraphicsService.h>
#include <Rhea/Core/Scene.h>
#include "MeshGpuCache.h"
#include "TextureGpuCache.h"
#include "MaterialGpuCache.h"

namespace Poseidon
{
	class Core;
}

namespace Rhea
{
	class Unit;
	class Element;
	class Event;	
}

namespace rhea::client {
	class GraphicsManager;
	class GraphicsScene;

	class GraphicsService : public Rhea::IGraphicsService
	{
		MECLASS(GraphicsService, Rhea::IGraphicsService)
	public:
		GraphicsService(std::string name, std::string type_name);
		~GraphicsService();

		// set the platform service
		void setPlatform(Rhea::Service* platform_service);

		// prepare a scene to display
		void onSceneActivate(Rhea::Scene* scene);
		void onSceneDeactivate(Rhea::Scene* scene);

		// show the targeted scene
		void showScene(Rhea::Scene* scene);
		void hideScene(Rhea::Scene* scene);

		// add item to render context
		void addItem(std::string render_context, Rhea::Item* item);

		// get the engine
		Poseidon::Core* getEngine() { return m_core.get(); }

		// Service events

		void onInit();

		bool onFrameFinal(float delta) override;
		
		bool onInput(Rhea::Event* ev, Rhea::Unit* unit);
		bool onAddItem(Rhea::Event* ev, Rhea::Unit* unit);
		
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
		bool addElement(Rhea::Unit* unit);

		MeshGpuCache& meshCache() { return m_meshCache; }
		TextureGpuCache& textureCache() { return m_textureCache; }
		MaterialGpuCache& materialCache() { return m_materialCache; }

	protected:
		std::unique_ptr<Poseidon::Core> m_core;
		Rhea::Service* m_platform;
		std::unordered_map<SimpleParser::Type*, std::unique_ptr<GraphicsManager>> m_managers;

		template<typename T>
		void addManager(std::unique_ptr<GraphicsManager> mgr) {
			m_managers[T::staticType()] = std::move(mgr);
		}
		
		// the activated scenes
		std::vector<Rhea::Scene* > m_scenes;

		// the current mouse pos
		int m_mx;
		int m_my;

		// fullscreen mode
		bool m_fullscreen;

		Rhea::Unit* pick();

		MeshGpuCache m_meshCache; 
		TextureGpuCache m_textureCache;
		MaterialGpuCache m_materialCache;
	};
}

#endif // !RHEA_GRAPHICS_SERVICE
