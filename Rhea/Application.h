#ifndef RHEA_H
#define RHEA_H

#include "Poseidon/Core.h"
#include "Psio/InputEvent.h"
#include "Orion/Orion.h"
#include "Orion/ResourceRequest.h"
#include "Resources/ResourceLoader.h"

#include <string>
#include <unordered_map>
#include <memory>

using ReqError = Orion::ResourceRequest::Error;

namespace Rhea
{	
	class Unit;
	class UnitTree;	
	class InputMap;
	class Dispatcher;
	class Controller;
	class Scene;
	class SceneView;
	class Service;
	class EventInput;
	class PhysicsService;

	class Application
	{
	public:
		// load and prepare all subsystems - calls setup at  
		void init();
		// set the applications name to appname
		void setAppname(std::string appname) { m_appname = appname; };
		// get a specific service
		Service* getService(std::string name);
		// register a service
		void registerService(std::string name, Service* service);
		// load and set up the resources of the application
		virtual void setup();
		// onScene Event
		virtual void onScene(Scene*) = 0;
		// the app main loop
		virtual int loop();
		// handle input - return true for quit
		virtual bool onInput(EventInput *e, Unit* unit);
		// onFrame Event
		virtual bool onFrameBegin() { return false; }
		virtual bool onFrameEnd() { return false; }
		// close the app
		virtual void close();
		// load a scene
		void loadScene(const std::string resourceId, std::function<void(Scene*, ReqError error)> handler);

		virtual void onResourceFailed(Orion::ResourceRequest::Error reason);

		// get a resource and load it as type
		void loadResource(const std::string resource_id, std::string type_name, std::function<void(Unit*, ReqError error)> handler);

		// get the resource loader
		ResourceLoader* getResourceLoader() { return &m_resource_loader; }

		// return the resource tree
		UnitTree* getUnitTree() { return m_unit_tree; }
		// get the action dispatcher
		Dispatcher* getDispatcher() { return m_dispatcher; }
		// set the directory for /local/
		void setAppDir(char* path);
		// log some info
		virtual void log(std::string message);

	protected:
		Orion::Orion* m_fs = 0;
		std::string m_appname = "";
		UnitTree* m_unit_tree = 0;
		InputMap* m_input_map = 0;
		Dispatcher* m_dispatcher = 0;
		Controller* m_focused_controller = 0;
		std::unordered_map<std::string, Service*> m_services;
		PhysicsService* m_physics = nullptr;

		ResourceLoader m_resource_loader;

		// timing info
		uint64_t current_ticks = 0;
		float fps = 0.0f;
	};
}
#endif // !RHEA_H
