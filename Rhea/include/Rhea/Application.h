/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_H
#define RHEA_H

#include <Psio/Input/InputEvent.h>
#include <Orion/Orion.h>
#include <Orion/ResourceRequest.h>
#include <Rhea/Resources/ResourceContext.h>
#include "Resources/ResourceLoader.h"
#include "FabricSystem.h"
#include "Graphics/Window.h"

#include <string>
#include <unordered_map>
#include <memory>

namespace rhea
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
	class ConfigFile;
	class IInputService;

	class Application
	{
	public:
		virtual ~Application();
		// load and prepare all subsystems - calls setup at  
		void init(int argc, char* argv[]);
		// set the applications name to appname
		void setAppname(std::string appname) { m_appname = appname; };
		// get the applications name
		std::string getAppname() { return m_appname; }
		// get a specific service
		Service* getService(std::string name);
		template <class T>
		T* getService() {
			for (auto& [name, service] : m_services) {
				if (doCast<T>(service))
					return doCast<T>(service);
			}
			return nullptr;
		}
		// register a service
		void registerService(std::string name, Service* service);
		void unregisterService(std::string name);
		// load and set up the resources of the application
		virtual void setup();
		// onScene Event
		virtual void onScene(Scene*) = 0;
		// set physics service
		void setPhysicsService(PhysicsService* physics) { m_physics = physics; }

		/**
		 * Main Application Loop
		 * ---------------------
		 * Orchestrates the application's core systems, including input, networking, physics, and rendering.
		 */
		virtual int loop();		

		/**
		 * handleInput
		 * -----------
		 * Processes a single input event.
		 * - Input events include player actions (e.g., key presses, mouse movement).
		 * - Updates relevant game state based on input.
		 * 
		 * Parameters:
		 * - `e`: A pointer to the `EventInput` object representing the current input event (e.g., key press, mouse movement).
		 * - `unit`: unused
		 * 
		 * Returns:
		 * - `true`: Signals that the engine should exit (e.g., user pressed a quit key or closed the window).
		 * - `false`: Continues normal processing of the game loop.
		 */
		virtual bool onInput(EventInput *e, Unit* unit);

		/**
		 * onFrameBegin
		 * -------------
		 * Called at the start of each frame.
		 * - Resets temporary state.
		 * - Hooks for profiling or debugging.
		 *
		 * Returns:
		 * - `true`: Signals that the engine should exit (e.g., user pressed a quit key or closed the window).
		 * - `false`: Continues normal processing of the game loop.
		 */
		virtual bool onFrameBegin() { return false; }

		/**
		 * onFrameEnd
		 * -----------
		 * Called at the end of the frame, after all updates, physics simulations, and rendering have completed.
		 *
		 * Responsibilities:
		 * - Perform any cleanup tasks that must occur at the very end of the frame.
		 * - Finalize the state of services or systems for the next frame.
		 * - Optionally trigger hooks for logging, debugging, or profiling.
		 *
		 * Examples of Use:
		 * - Resetting temporary or frame-local data structures.
		 * - Flushing rendering buffers or completing GPU operations.
		 * - Preparing debug overlays or metrics for the next frame.
		 *
		 * Notes:
		 * - `onFrameEnd()` is the last step of the game loop before it starts processing the next frame.
		 * - It should not contain gameplay-critical logic, as the frame has already been rendered.
		 *
		 * Returns:
		 * - `true`: Signals that the engine should exit (e.g., user pressed a quit key or closed the window).
		 * - `false`: Continues normal processing of the game loop.

		 */
		virtual bool onFrameEnd() { return false; }

		// close the app
		virtual void close();

		virtual void onResourceFailed(orion::Error reason);

		// get a resource and load it as type
		void loadResource(const std::string& resource_id, const std::string& type_name, std::function<void(Component*, ErrorReport error)> handler);
		void loadResource(const std::string& resource_id, const ResourceContext& context, const std::string& type_name, std::function<void(Component*, ErrorReport error)> handler);
		void completeLoadResource(const std::string& resource_id, const ResourceContext& context, orion::ResourceRequest* req, orion::Error err);

		// get the resource loader
		ResourceLoader* getResourceLoader() { return &m_resource_loader; }

		// return the resource tree
		UnitTree* getUnitTree() { return m_unit_tree.get(); }
		// get the action dispatcher
		Dispatcher* getDispatcher() { return m_dispatcher; }
		// set the directory for /local/
		void setAppDir(const char* path);
		// log some info
		virtual void log(std::string message);

		std::string resourceUriToId(const std::string& resource_id);

		// get fs
		orion::Orion* getFS() { return m_fs; }

		// register a fabric
		void RegisterFabric(std::unique_ptr<IFabric> fabric);
		// init fabrics - call this after registering to initialize all
		void InitFabrics();

		ConfigFile* getConfig();

		const std::vector<Window>& getWindows() const { return m_windows; }
		void addWindow(Window win) { m_windows.push_back(win); }
		// helper add default window
		void addDefaultWindow();

		virtual void onResizeWindow(int width, int height);

	protected:
		orion::Orion* m_fs = 0;
		std::string m_appname = "";
		std::unique_ptr<UnitTree> m_unit_tree;
		InputMap* m_input_map = 0;
		Dispatcher* m_dispatcher = 0;
		Controller* m_focused_controller = 0;
		std::unordered_map<std::string, Service*> m_services;
		PhysicsService* m_physics = nullptr;
		Service* m_wold_service = nullptr;
		IInputService* m_input_service = nullptr;
		ConfigFile* m_config = nullptr;
		// Windows
		std::vector<Window> m_windows;

		using LoadHandler = std::function<void(Component*, ErrorReport)>;
		void finishResourceLoad(const std::string& rid, Component* component, ErrorReport report);
		std::unordered_map<std::string, std::vector<LoadHandler>> m_pending_resource_loads;

		ResourceLoader m_resource_loader;

		// timing info
		uint64_t elapsed_ticks = 0;
		float fps = 0.0f;

	private:
		std::vector<std::unique_ptr<IFabric>> m_fabrics;
	};
}
#endif // !RHEA_H
