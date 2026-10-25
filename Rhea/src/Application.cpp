/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <exception>
#include <algorithm>

#include "Orion/Orion.h"
#include "Orion/ResourceStream.h"
#include "Orion/ResourceRequest.h"
#include <Orion/Mime.h>
#include "Psio/Psio.h"
#include "Psio/Log.h"
#include "Rhea/Core/SceneLoader.h"
#include "Rhea/Unit.h"
#include "Rhea/UnitTree.h"
#include "Rhea/Realm.h"
#include "Rhea/Core/Item.h"
#include "Rhea/Core/Transform.h"
#include "Rhea/Core/Scene.h"
#include "Rhea/Core/Mesh.h"
#include "Rhea/Core/Camera.h"
#include "Rhea/Core/Item2D.h"
#include "Rhea/Core/AreaService.h"
#include "Rhea/Core/AreaComponent.h"
#include "Rhea/Core/Trigger.h"
#include "Rhea/Resources/TextResource.h"
#include "Rhea/Core/SceneResource.h"
#include "Rhea/Resources/CubeMap.h"
#include "Rhea/Resources/TextLoader.h"
#include "Rhea/Resources/ImageResource.h"
#include "Rhea/Input/InputMap.h"
#include "Rhea/Input/IInputService.h"
#include "Rhea/Dispatcher.h"
#include "Rhea/Resources/ResourceService.h"

#include "Rhea/Platform/PlatformService.h"
#include "Rhea/Physics/PhysicsManagerService.h"
#include "Rhea/Physics/PhysicsService.h"

#include "SimpleParser/Type.h"
#include "SimpleParser/Method.h"

#include "Rhea/Application.h"
#include <Rhea/EventInput.h>
#include <Rhea/Core/Avatar.h>
#include <Rhea/Resources/Resources.h>
#include <Rhea/Resources/ResourceLibrary.h>
#include <Rhea/Resources/Collection.h>
#include <Rhea/Core/Shader.h>
#include <Rhea/Core/WorldService.h>
#include <Rhea/Core/World.h>
#include "Rhea/Core/Light.h"
#include "Rhea/Core/DirectionalLight.h"
#include <Rhea/Core/Lighting.h>
#include "Rhea/Core/PointLight.h"
#include "Rhea/Core/SpotLight.h"
#include <Rhea/Component.h>
#include <Rhea/Core/Geometry.h>
#include <Rhea/ConfigFile.h>
#include <Rhea/Components/MeshComponent.h>
#include <Rhea/Components/PhysicsDataComponent.h>
#include <Rhea/Core/SceneInstance.h>

#undef min

extern std::vector<std::unique_ptr<rhea::IFabric>> createFabrics();

namespace rhea
{
	Application::~Application()
	{
		for (auto it = m_services.begin(); it != m_services.end(); ++it)
		{
			delete it->second;
		}
		m_services.clear();
		m_unit_tree.reset();
	}

	void Application::init(int argc, char* argv[])
	{
		// set log severity
#ifdef _DEBUG
		Log::logger().setLevel(LOG_DEBUG);
#else
		Log::logger().setLevel(LOG_INFO);
#endif // DEBUG

		// register core types
		simpleparser::Type::registerType<Component>("//core/types/component");
		simpleparser::Type::registerType<Unit>("//core/types/unit");
		simpleparser::Type::registerType<UnitTree>("//core/unittree");
		simpleparser::Type::registerType<Collection>("//core/collection");
		simpleparser::Type::registerType<ConfigFile>("//core/types/configfile");
		SceneResource::_register_SceneResource();

		// register core components
		VisualComponent::_register_VisualComponent();
		MeshComponent::_register_MeshComponent();
		PhysicsDataComponent::_register_PhysicsDataComponent();
		ColliderComponent::_register_ColliderComponent();
		Transform::_register_Transform();
		// register world framework components
		AreaComponent::_register_AreaComponent();
		SceneEntryPoint::_register_SceneEntryPoint();

		// register services
		simpleparser::Type::registerType<Service>("//meworla/service");
		simpleparser::Type::registerType<PlatformService>("//meworla/platformservice");
		simpleparser::Type::registerType<PhysicsManagerService>("//meworla/physicsmanagerservice");
		simpleparser::Type::registerType<WorldService>("//meworla/worldservice");
		simpleparser::Type::registerType<ResourceService>("//meworla/resourceservice");

		m_unit_tree.reset((UnitTree*)simpleparser::Type::create("/", "//core/unittree"));
		m_unit_tree->setup(this);

		ConfigFile* config = simpleparser::Type::create<ConfigFile>("config");
		config->setup(this);
		m_unit_tree->addChild(config);
		config->fromCommandLineArgs(argc, argv);

		Service* psio = (PlatformService*)simpleparser::Type::create("PlatformService", "//meworla/platformservice");
		std::string psio_backend = config->getValueString("platform.backend", "auto");
		psio->setValue("backend", psio_backend);
		psio->setup(this);
		m_services["platform"] = psio;

		m_fs = new orion::Orion(static_cast<PlatformService*>(psio)->getEngine(), m_appname);

		std::string app_dir = config->getValueString("appdir", "");
		if (!app_dir.empty()) {
			setAppDir(app_dir.c_str());
		}

		// load config file
		config->loadFromFile("config.ini");

		// log-level from config
#ifdef _DEBUG
		std::string log_level = config->getValueString("log.level", "debug");
#else
		std::string log_level = config->getValueString("log.level", "info");
#endif
		std::transform(log_level.begin(), log_level.end(), log_level.begin(), ::tolower);
		if (log_level == "debug")
			Log::logger().setLevel(LOG_DEBUG);
		else if (log_level == "info")
			Log::logger().setLevel(LOG_INFO);
		else if (log_level == "warning")
			Log::logger().setLevel(LOG_WARNING);
		else if (log_level == "error")
			Log::logger().setLevel(LOG_ERROR);
		else {
			printf("Invalid log level in config: %s\n", log_level.c_str());
		}

		Service* world = (WorldService*)simpleparser::Type::create("WorldService", "//meworla/worldservice");
		world->setup(this);
		m_services["world"] = world;

		Service* phy_mgr = (PhysicsManagerService*)simpleparser::Type::create("PhysicsManagerService", "//meworla/physicsmanagerservice");
		phy_mgr->setup(this);
		m_services["physics_mgr"] = phy_mgr;

		Service* res_service = (Service*)simpleparser::Type::create("ResourceService", "//meworla/resourceservice");
		m_services["resources"] = res_service;
		simpleparser::Type::registerType<Resource>("//meworla/resource");

		// register generic text loader
		simpleparser::Type::registerType<TextResource>("//meworla/text");
		m_resource_loader.registerLoader<TextLoader>("text/*");

		// register scene types
		simpleparser::Type::registerType<Realm>("//meworla/realm");
		simpleparser::Type::registerType<Scene>("//meworla/scene");
		m_resource_loader.registerLoader<SceneLoader>("application/vnd.meworla.scene");
		simpleparser::Type::registerType<Resources>("//meworla/resources");
		ResourceLibrary::_register_ResourceLibrary();
		simpleparser::Type::registerType<Transform>("//meworla/transform");
		Item::_register_Item();
		simpleparser::Type::registerType<Mesh>("//meworla/mesh");
		simpleparser::Type::registerType<Shader>("//meworla/shader");
		simpleparser::Type::registerType<Lighting>("//meworla/lighting");
		Light::_register_Light();
		simpleparser::Type::registerType<PointLight>("//meworla/pointlight");
		simpleparser::Type::registerType<SpotLight>("//meworla/spotlight");
		simpleparser::Type::registerType<DirectionalLight>("//meworla/directionallight");
		orion::MIME::RegisterExtenstion("application/vnd.meworla.scene", "shader");
		simpleparser::Type::registerType<Material>("//meworla/material");
		simpleparser::Type::registerType<Avatar>("//meworla/avatar");
		SceneInstance::_register_SceneInstance();
		World::_register_World();
		Area::_register_Area();
		AreaService::_register_AreaService();
		auto* area_srv = new AreaService("area");
		area_srv->setup(this);
		m_services["area"] = area_srv;
		Trigger::_register_Trigger();
		ImageResource::_register_ImageResource();
		simpleparser::Type::registerType<ImageResource>("//meworla/texture");
		CubeMap::_register_CubeMap();

		Camera::_register_Camera();
		simpleparser::Type::registerType<Item2D>("//meworla/widget");
		simpleparser::Type::registerType<TextResource>("//meworla/fontinfo");
		orion::MIME::RegisterExtenstion("text/fontinfo", "fnt");

		//GUIController::Register(this);

		simpleparser::Type::registerType<Geometry>("//meworla/geometry");

		// register mime type for shader
		orion::MIME::RegisterExtenstion("text/plain", "hlsl");
		orion::MIME::RegisterExtenstion("text/plain", "glsl");

		// register optional fabrics
		for (auto& mod : createFabrics()) {
			RegisterFabric(std::move(mod));
		}
		InitFabrics();

		// get the input service and set the input handler
		m_input_service = doCast<IInputService>(m_services["input"]);

		// add a default input map
		m_input_map = new InputMap();
		m_input_map->loadDefaultMap();
		m_dispatcher = new Dispatcher();

		// create a representation for the engine
		rhea::Unit* engine = doCast<Unit>(m_unit_tree->createUnit("engine", "//core/types/unit", {}, {}));
		m_unit_tree->addChild(engine);
	}

	Service* Application::getService(std::string name)
	{
		auto srv = m_services.find(name);
		if (srv == m_services.end())
			return nullptr;
		return srv->second;
	}

	void Application::registerService(std::string name, Service* service)
	{
		m_services[name] = service;
	}

	void Application::unregisterService(std::string name)
	{
		m_services.erase(name);
	}

	void rhea::Application::addDefaultWindow()
	{
		Window default_window;
		default_window.name = "main";
		default_window.title = m_appname;
		if (!default_window.title.size())
			default_window.title = "Rhea Application";
		auto* config = getConfig();
		if (config) {
			default_window.width = config->getValue("window.width", 1152);
			default_window.height = config->getValue("window.height", 864);
		}
		else {
			LOG(LOG_WARNING) << "No window defined and no config file found, using default window size 800x600";
			default_window.width = 800;
			default_window.height = 600;
		}
		m_windows.push_back(default_window);
		m_windows[0].views.push_back({ "default", 0,0,default_window.width,default_window.height });
		m_windows[0].views[0].layers.push_back({ "default", 0,0,default_window.width,default_window.height });
	}

	void Application::setup()
	{
		// if no window and no view was defined we create a default one
		if (!m_windows.size()) {
			addDefaultWindow();
		}

		for (auto it = m_services.begin(); it != m_services.end(); ++it)
		{
			Service* s = it->second;
			s->onInit();
		}
	}

	void Application::setAppDir(const char* path)
	{
		if (!m_fs) {
			LOG(LOG_FATAL) << "Error: SetAppDir called before App::init()";
			exit(-1);
		}
		m_fs->setLocalDir(path);
		m_services["platform"]->setValue("AppDir", simpleparser::Variant(path));
	}

	void Application::onResourceFailed(orion::Error reason)
	{
		LOG(LOG_ERROR) << "Resource loading failed";
	}

	bool Application::onInput(EventInput* e, Unit* unit)
	{
		// close on esc
		if (e->isKeyPressed(rhea::Key::Escape) || e->isAction("cancel")) {
			return true;
		}
		return false;
	}

	/**
	 * Main Application Loop
	 * --------------
	 * This loop drives the application by managing input, network, physics, and other subsystems.
	 *
	 * Key Components:
	 * - `elapsed_s`: Time in seconds since the last frame. Used for frame-based updates.
	 * - `frame_slice`: Fixed timestep for physics updates. Ensures stability and determinism.
	 *
	 * Steps:
	 * 1. Calculate `elapsed_s` to determine the time since the last frame.
	 * 2. Begin frame processing (e.g., resetting state, profiling hooks)
	 * 3. Handle input events and io/network updates.
	 * 4. Perform pre-physics updates (frame-based, using `elapsed_s`).
	 * 5. Run physics simulation in fixed timesteps (`frame_slice`).
	 * 6. Perform frame-based updates (animations, AI, etc., using `elapsed_s`).
	 * 7. Perform final updates (e.g., rendering).
	 * 8. Finish frame processing after all updates, physics simulations, and rendering have completed.
	 */
	int Application::loop()
	{
		// start the loop
		int ret = 0;
		psio::Engine* psio = static_cast<PlatformService*>(m_services["platform"])->getEngine();

		int iterations_per_second = 60;
		float frame_slice = 1.0f / iterations_per_second;
		uint64_t current_ticks = 0;
		uint64_t last_ticks = psio->getTicks();

		uint64_t frame = 0;
		int physics_steps = 0;
		float accumulated_time = 0; // time accumulated time since last physical steps
		//float idle_step;

		fps = 0.0f;
		uint64_t frames = 0;

		uint64_t fps_start = psio->getTicks();
		uint64_t fps_count = 0;
		float elapsed_fs = 0.0f;

		while (!ret) {

			// timings

			// 1. Calculate elapsed time since the last frame (in seconds)
			current_ticks = psio->getTicks();
			elapsed_ticks = current_ticks - last_ticks;
			float elapsed_s = static_cast<float>(elapsed_ticks) / 1000000.0f;
			last_ticks = current_ticks;

			// compute fps
			fps = 1 / elapsed_s; // fps for current frame
			elapsed_fs += elapsed_s;
			//  show all
			if (elapsed_fs > 1.0f) {
				fps = (frames - fps_count) / elapsed_fs;
				elapsed_fs = 0.0f;
				fps_count = frames;
			//	printf("fps: %6.1f\r", fps);
			}

			// 1 physics step handles [frame_slice] time
			// we need to compute how many steps have to be taken to "catch up"
			accumulated_time += elapsed_s;
			physics_steps = (int)std::floor(accumulated_time * iterations_per_second);
			accumulated_time -= physics_steps * frame_slice;
			//if (physics_steps > 5)
			//	printf("phys steps: %d ; acc: %f        \r\n", physics_steps, accumulated_time);

			// 2. Begin frame processing (e.g., resetting state, profiling hooks)
			if (onFrameBegin())
				ret = true;

			// 3.1 check for window and input events
			if (m_input_service) {

				m_input_service->update();

				InputEvent evt;
				while (m_input_service->pollEvent(evt)) {
					// always handle OS quit
					if (evt.isQuit()) {
						return true;
					}
					// get actions from input map
					std::vector<std::string> actions = m_input_map->getActions(evt);
					// send the input events
					// TODO: make the event list configurable
					EventInput e("input", evt, actions);
					m_unit_tree->sendEvent("input", &e);
					if (!e.isPropagationStopped())
						m_unit_tree->sendEvent("gui_input", &e);
					if (!e.isPropagationStopped())
						m_unit_tree->sendEvent("unhandled_input", &e);
					if (!e.isPropagationStopped())
						m_unit_tree->sendEvent("scene_input", &e);

					if (!e.isPropagationStopped())
						ret = onInput(&e, nullptr);
				}
			}

			// 3.2 Run io and network updates (e.g., sync data, process packets)
			psio->RunNet();
			m_fs->poll();

			// 4. Call pre-physics updates for all services (elapsed time is frame-based)
			for (auto it = m_services.begin(); it != m_services.end(); ++it) {
				if (it->second->onPrePhysics(elapsed_s))
					ret = true;
			}

			// physics

			// 5. Run fixed-timestep physics simulation
			uint64_t physics_begin = psio->getTicks();

			if (m_physics || m_wold_service) {
				static int max_physics_steps = 10;
				physics_steps = std::min(physics_steps, max_physics_steps);

				for (int i = 0; i < physics_steps; i++)
				{
					if (m_physics)
						m_physics->onPhysicsStep(frame_slice);
					if (m_wold_service)
						m_wold_service->onPhysicsStep(frame_slice);
				}
			}

			// idle
			uint64_t idle_begin = psio->getTicks();

			// idle(elapsed_s)

			// 6. Call frame-based updates for all services - physics is done
			for (auto it = m_services.begin(); it != m_services.end(); ++it) {
				if (it->second->onFrame(elapsed_s))
					ret = true;
			}

			// 7. Perform any final updates (e.g., rendering, cleanup)
			for (auto it = m_services.begin(); it != m_services.end(); ++it) {
				if (it->second->onFrameFinal(elapsed_s))
					ret = true;
			}

			frames++;

			// 8. Finish the frame, after all updates, physics simulations, and rendering have completed.
			if (onFrameEnd())
				ret = true;
		}

		return ret;
	}

	void Application::loadResource(const std::string& resource_id, const std::string& type_name, std::function<void(Component*, Error error)> handler)
	{
		loadResource(resource_id, {}, type_name, handler);
	}


	void Application::loadResource(const std::string& resource_id, const ResourceContext& context, const std::string& type_name, std::function<void(Component*, Error error)> handler)
	{
		std::string rid = context.normalizeResourceId(resource_id);

		log("loading resource: " + rid);
		orion::ResourceRequest* req = m_fs->getResourceByRid(rid);
		req->resolve([this, handler, context, rid, resource_id](orion::ResourceRequest* req, orion::Error err) {

			if (err == orion::Error::OK) {

				// we got the resource

				// store current context and set the new one for the resource
				std::string realm_name = req->getRealmName();
				ResourceContext loaded_context = context;
				loaded_context.source_directory = "//" + req->getRealmName() + req->getParentPath();
				if (loaded_context.context_root.empty())
					loaded_context.context_root = loaded_context.source_directory;

				// get the realm parent unit (or create it if it doesn't exists)
				Component* realm_unit = m_unit_tree->find(realm_name, false);
				if (!realm_unit) {
					// add the realm
					realm_unit = (Component*)new Realm(req->getRealm(), realm_name);
					if (!m_unit_tree->addChild(realm_unit)) {
						LOG(LOG_ERROR) << "LoadResource(" << resource_id << "): Failed to add realm " << realm_name;
						delete realm_unit;
						if (handler)
							handler(NULL, Error(Error::Code::INVALID_DATA, "Failed to add realm " + realm_name, rid));
						return;
					}
				}

				// find the unit parent and create the folders if necessary
				std::string parentpath = realm_name + req->getParentPath();
				Unit* parent_unit = doCast<Unit>(m_unit_tree->find(parentpath, true));
				if (!parent_unit) {
					// should not happen
					if (handler)
						handler(NULL, Error(Error::Code::NOT_FOUND, "Parent unit not found: " + parentpath, rid));
					return;
				}

				if (req->getStream()->IsDir()) {
					// sepcial case for directory
					Collection* collection = new Collection(req->getBasename(), "//core/collection");
					collection->readFromStream(req->getStream());
					if (!parent_unit->addChild(collection)) {
						std::string path = collection->getPathName();
						LOG(LOG_ERROR) << "LoadResource(" << resource_id << "): Failed to add collection " << path << " to parent " << parent_unit->getPath();
						delete collection;
						if (handler)
							handler(NULL, Error(Error::Code::INVALID_DATA, "Failed to add collection " + path + " to parent " + parent_unit->getPath(), rid));
						return;
					}
					if (handler)
						handler(collection, Error());
					return;
				}

				// call loader for resource mime type

				// get from request
				std::string mime = req->getMimeType();
				if (mime.length()) {
					if (m_resource_loader.load(req->getBasename(), this, mime, req, loaded_context, [this, handler, parent_unit, rid](Error err, Component* component) {

						if (err.isError()) {
							if (component) {
								LOG(LOG_ERROR) << "LoadResource(" << rid << "): Failed to load component " << component->getPathName();
								delete component;
							}
							if (handler)
								handler(NULL, err);
							return;
						}

						if (!component) {
							if (handler) {
								handler(nullptr, Error(Error::Code::INVALID_DATA, "Resource loader returned no component.", rid));
							}
							return;
						}

						// add it to tree
						if (!parent_unit->addChild(component)) {
							std::string path = component->getPathName();
							LOG(LOG_ERROR) << "LoadResource(" << rid << "): Failed to add component " << path << " to parent " << parent_unit->getPath();
							delete component;
							if (handler)
								handler(NULL, Error(Error::Code::INVALID_DATA, "Failed to add component " + path + " to parent " + parent_unit->getPath(), rid));
							return;
						}

						if (handler)
							handler(component, Error());
						}))
						return;
						log("No loader for mime type " + mime);
				}
				else
					log("No mime type for " + rid);

				// this is only called for unhandled or unknown mime types

				// create a generic resource unit
				Resource* component = new Resource(req->getBasename(), "//meworla/resource");

				// call load of unit
				component->load(req, [parent_unit, component, handler, rid](Error err) {
					if (err.isError()) {
						std::string path = component->getPathName();
						delete component;
						LOG(LOG_ERROR) << "LoadResource(" << rid << "): Failed to load component " << path;
						if (!handler)
							return;
						return handler(NULL, err);
					}

					// add it to tree
					if (!parent_unit->addChild(component)) {
						std::string path = component->getPathName();
						LOG(LOG_ERROR) << "LoadResource(" << rid << "): Failed to add component " << path << " to parent " << parent_unit->getPath();
						delete component;
						if (!handler)
							return;
						return handler(NULL, Error(Error::Code::INVALID_DATA, "Failed to add component " + path + " to parent " + parent_unit->getPath(), rid));
					}

					if (handler)
						handler(component, Error());
					});
			}
			else
				if (handler)
					handler(NULL, translateLoadError(err, rid));
			});
	}

	void Application::onResizeWindow(int width, int height)
	{

	}

	void Application::log(std::string message)
	{
		LOG(LOG_INFO) << message;
	}

	void Application::close()
	{

	}

	void Application::RegisterFabric(std::unique_ptr<IFabric> fabric) {
		LOG(LOG_INFO) << "[Application] Registered fabric: " << fabric->getName();
		m_fabrics.push_back(std::move(fabric));
	}

	void Application::InitFabrics() {
		LOG(LOG_INFO) << "[Application] Initializing fabrics...";
		for (auto& fabric : m_fabrics) {
			LOG(LOG_INFO) << " - Initializing: " << fabric->getName();
			fabric->Register(this);
		}
	}

	ConfigFile* Application::getConfig()
	{
		ConfigFile* config = doCast<rhea::ConfigFile>(m_unit_tree->getChild("config"));
		return config;
	}
}