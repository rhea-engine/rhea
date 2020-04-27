#ifndef RHEA_H
#define RHEA_H

#include <string>
#include <unordered_map>
#include <memory>

#include "Poseidon/Core.h"
#include "Psio/InputEvent.h"
#include "Orion/Orion.h"
#include "Orion/ResourceRequest.h"

using ReqError = Orion::ResourceRequest::Error;

namespace Rhea
{	
	class Unit;
	class UnitTree;	
	class InputMap;
	class Dispatcher;

	class Application
	{
	public:
		// load and prepare all subsystems - calls setup at  
		void init();
		// set the applications name to appname
		void setAppname(std::string appname) { m_appname = appname; };
		Poseidon::Core* get_core() { return core; };
		// load and set up the resources of the application
		virtual void setup();
		// the app main loop
		virtual int loop();
		// handle input - return true for quit
		virtual bool onInput(std::shared_ptr<Psio::InputEvent> evt);
		// onFrame Event
		virtual bool onFrameBegin() { return false; }
		virtual bool onFrameEnd() { return false; }
		// close the app
		virtual void close();
		// load a scene
		void loadScene(const std::string resourceId);
		// scene was loaded callback
		void onSceneReady(Unit *parent, Orion::Resource*);
		virtual void onResourceFailed(Orion::ResourceRequest::Error reason);
        // get a resource
		void loadResource(const std::string resource_id, Orion::ResourceRequest::RequestHandler handler);
		// get a resource and load it as type
		void loadResource(const std::string resource_id, std::string type_name, std::function<void(Unit*, ReqError error)> handler);
		// return the reosurce tree
		UnitTree* getUnitTree() { return m_unit_tree; }
		// get the action dispatcher
		Dispatcher* getDispatcher() { return m_dispatcher; }
		// set the directory for /local/
		void setAppDir(char* path);
		// log some info
		virtual void log(std::string message);

	protected:
		Poseidon::Core* core = 0;
		Orion::Orion* m_fs = 0;
		std::string m_appname = "";
		UnitTree* m_unit_tree;
		InputMap* m_input_map = 0;
		Dispatcher* m_dispatcher = 0;
	};
}
#endif // !RHEA_H
