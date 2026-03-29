#include <exception>
#include <algorithm>

#include "Orion/Orion.h"
#include "Orion/ResourceStream.h"
#include "Orion/ResourceRequest.h"
#include <Orion/Mime.h>
#include "Psio/Psio.h"
#include "Psio/InputEvent.h"
#include "Psio/Keyboard.h"
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
#include "Rhea/Resources/TextResource.h"
#include "Rhea/Resources/CubeMap.h"
#include "Rhea/Resources/TextLoader.h"
#include "Rhea/Resources/ImageResource.h"
#include "Rhea/InputMap.h"
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
#include <Rhea/Resources/Collection.h>
#include <Rhea/Core/Shader.h>
#include <Rhea/Core/WorldService.h>
#include "Rhea/Core/Light.h"
#include "Rhea/Core/DirectionalLight.h"
#include <Rhea/Core/Lighting.h>
#include "Rhea/Core/PointLight.h"
#include "Rhea/Core/SpotLight.h"
#include <Rhea/Component.h>
#include <Rhea/Core/Geometry.h>
#include <Rhea/ConfigFile.h>
//#include <Physics/PhysicsComponent.h>

#undef min

extern std::vector<std::unique_ptr<Rhea::IFabric>> createFabrics();

namespace Rhea
{
	Application::~Application()
	{
		m_unit_tree.reset();
		for (auto it = m_services.begin(); it != m_services.end(); ++it)
		{
			delete it->second;
		}
		m_services.clear();
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
		SimpleParser::Type::registerType<Component>("//core/types/component"); 
		SimpleParser::Type::registerType<Unit>("//core/types/unit");
		SimpleParser::Type::registerType<UnitTree>("//core/unittree");
		SimpleParser::Type::registerType<Collection>("//core/collection");
		SimpleParser::Type::registerType<ConfigFile>("//core/types/configfile");

		// register services
		SimpleParser::Type::registerType<Service>("//meworla/service");
		SimpleParser::Type::registerType<PlatformService>("//meworla/platformservice");
		SimpleParser::Type::registerType<PhysicsManagerService>("//meworla/physicsmanagerservice");
		SimpleParser::Type::registerType<WorldService>("//meworla/worldservice");
		SimpleParser::Type::registerType<ResourceService>("//meworla/resourceservice");

		m_unit_tree.reset((UnitTree*)SimpleParser::Type::create("/", "//core/unittree"));
		m_unit_tree->setup(this);

		Service* psio = (PlatformService*)SimpleParser::Type::create("PlatformService", "//meworla/platformservice");
		psio->setup(this);
		m_services["platform"] = psio;

		m_fs = new Orion::Orion(static_cast<PlatformService*>(psio)->getEngine(), m_appname);

		// load config file
		ConfigFile* config = SimpleParser::Type::create<ConfigFile>("config");
		config->setup(this);
		m_unit_tree->addChild(config);
		config->loadFromFile("config.ini");
		config->fromCommandLineArgs(argc, argv);

		// log-level from config
		std::string log_level = config->getValue("log.level", "info");
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

		Service* world = (WorldService*)SimpleParser::Type::create("WorldService", "//meworla/worldservice");
		world->setup(this);
		m_services["world"] = world;

		Service* phy_mgr = (PhysicsManagerService*)SimpleParser::Type::create("PhysicsManagerService", "//meworla/physicsmanagerservice");
		phy_mgr->setup(this);
		m_services["physics_mgr"] = phy_mgr;		

		Service* res_service = (Service*)SimpleParser::Type::create("ResourceService", "//meworla/resourceservice");
		m_services["resources"] = res_service;
		SimpleParser::Type::registerType<Resource>("//meworla/resource");

		// register generic text loader
		SimpleParser::Type::registerType<TextResource>("//meworla/text");
		m_resource_loader.registerLoader<TextLoader>("text/*");

		// register scene types
		SimpleParser::Type::registerType<Realm>("//meworla/realm");
		SimpleParser::Type::registerType<Scene>("//meworla/scene");
		m_resource_loader.registerLoader<SceneLoader>("application/vnd.meworla.scene");
		SimpleParser::Type::registerType<Resources>("//meworla/resources");
		SimpleParser::Type::registerType<Transform>("//meworla/transform");
		SimpleParser::Type::registerType<Item>("//meworla/item");
		SimpleParser::Type::registerType<Mesh>("//meworla/mesh");				
		SimpleParser::Type::registerType<Shader>("//meworla/shader");
		SimpleParser::Type::registerType<Lighting>("//meworla/lighting");
		SimpleParser::Type::registerType<Light>("//meworla/light");
		SimpleParser::Type::registerType<PointLight>("//meworla/pointlight");
		SimpleParser::Type::registerType<SpotLight>("//meworla/spotlight");
		SimpleParser::Type::registerType<DirectionalLight>("//meworla/directionallight");
		Orion::MIME::RegisterExtenstion("application/vnd.meworla.scene", "shader");
		SimpleParser::Type::registerType<Material>("//meworla/material");
		SimpleParser::Type::registerType<Avatar>("//meworla/avatar");
		
		ImageResource::_register_ImageResource();
		SimpleParser::Type::registerType<ImageResource>("//meworla/texture");
		CubeMap::_register_CubeMap();

		SimpleParser::Type::registerType<Camera>("//meworla/camera");
		SimpleParser::Type::registerType<Item2D>("//meworla/widget");
		SimpleParser::Type::registerType<TextResource>("//meworla/fontinfo");
		//GUIController::Register(this);
		
		SimpleParser::Type::registerType<Geometry>("//meworla/geometry");

		// register mime type for shader
		Orion::MIME::RegisterExtenstion("text/plain", "hlsl");
		Orion::MIME::RegisterExtenstion("text/plain", "glsl");

		// register optional fabrics
		for (auto& mod : createFabrics()) {
			RegisterFabric(std::move(mod));
		}
		InitFabrics();

		// add a default input map
		m_input_map = new InputMap();
		m_input_map->loadDefaultMap();
		m_dispatcher = new Dispatcher();
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

	void Application::setup()
	{		
		for (auto it = m_services.begin(); it != m_services.end(); ++it)
		{
			Service* s = it->second;
			s->onInit();			
		}
	}

	void Application::setAppDir(const char* path)
	{
		if (!m_fs) {
			printf("Error: SetAppDir called before App::init()\n");
			exit(-1);
		}
		m_fs->setLocalDir(path);
		m_services["platform"]->setValue("AppDir", SimpleParser::Variant(path));
	}

	void Application::onResourceFailed(Orion::ResourceRequest::Error reason)
	{
		printf("Resource loading failed\n");
	}

	bool Application::onInput(EventInput* e, Unit* unit)
	{
		std::shared_ptr<Psio::InputEvent> evt = e->getEvent();

		// close on esc
		Psio::KeyInputEvent esc(Psio::InputEventId::KEY_PRESSED);
		esc.set(Psio::Key::CODE_ESC, 0, 0);

		if (evt->isEvent(&esc)) {
			return true;
		}

		// toggle physics service
		esc.set(Psio::Key::CODE_F9, 0, 0);
		if (evt->isEvent(&esc)) {
			PhysicsManagerService* mgr = static_cast<PhysicsManagerService*>(m_services["physics_mgr"]);
			if (mgr && mgr->getPhysicsService()) mgr->getPhysicsService()->toggleDebug();
		}

		// pressed mouse button
		Psio::MouseInputEvent btn(Psio::InputEventId::MOUSE_BTN_PRESSED);


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
	 * 3. Handle input events and network updates.
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
		Psio::Engine* psio = static_cast<PlatformService*>(m_services["platform"])->getEngine();
		std::shared_ptr<Psio::InputEvent> evt;
		ievt_actions iea;		

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
			while (evt = psio->pollInput()) {

				// always handle OS quit
				if (evt->getId() == Psio::InputEventId::WMGR_QUIT) {
					return true;
				}

				// get actions from input map
				iea = m_input_map->getActions(evt);

				// send the input events
				// TODO: make the event list configurable
				EventInput e("input", evt, iea.actions);
				m_unit_tree->sendEvent("input", &e);
				if (!e.isPropagationStoppped())
					m_unit_tree->sendEvent("gui_input", &e);
				if (!e.isPropagationStoppped())
					m_unit_tree->sendEvent("unhandled_input", &e);
				if (!e.isPropagationStoppped())
					m_unit_tree->sendEvent("scene_input", &e);

				// application input handling comes last
				if (onInput(&e, nullptr))
					ret = true;
			}

			// 3.2 Run network updates (e.g., sync data, process packets)
			psio->RunNet();
			
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

	void Application::loadResource(const std::string resource_id, std::string type_name, std::function<void(Unit*, ReqError error)> handler)
	{
		log("loading resource: " + resource_id);
		Orion::ResourceRequest* req = m_fs->getResourceByRid(resource_id);
		req->resolve([&, req, handler, resource_id](Orion::ResourceRequest* req, int err) {

			if (err == Orion::ResourceRequest::Error::OK) {

				// we got the resource

				// get the realm parent unit (or create it if it doesn't exists) 
				std::string realm_name = req->getRealmName();
				Component* realm_unit = m_unit_tree->find(realm_name, false);
				if (!realm_unit) {
					// add the realm
					realm_unit = (Unit*)new Realm(req->getRealm(), realm_name);
					m_unit_tree->addChild(realm_unit);
				}

				// find the unit parent and create the folders if necessary
				std::string parentpath = realm_name + req->getParentPath();
				Unit* parent_unit = doCast<Unit>(m_unit_tree->find(parentpath, true));
				if (!parent_unit)
					// should not happen
					return handler(NULL, ReqError::PATH_NOT_FOUND);

				if (req->getStream()->IsDir()) {
					// sepcial case for directory
					Collection* collection = new Collection(req->getBasename(), "//core/collection");
					collection->readFromStream(req->getStream());
					parent_unit->addChild(collection);
					return handler(collection, ReqError::OK);
				}

				// call loader for resource mime type

				// get from request
				std::string mime = req->getMimeType();
				if (mime.length()) {
					if (m_resource_loader.load(req->getBasename(), this, mime, req, [&, handler, parent_unit](ReqError err, Unit* unit) {

						// add it to tree
						if (unit)
							parent_unit->addChild(unit);

						handler(unit, err);
						}))
						return;
						log("No loader for mime type " + mime);
				}
				else
					log("No mime type for " + resource_id);

				// this is only called for unhandled or unknown mime types

				// create a generic resource unit
				Resource* unit = new Resource(req->getBasename(), "//meworla/resource");

				// add it to tree
				parent_unit->addChild(unit);

				// call load of unit
				unit->load(req, [&, handler](ReqError err) {
					handler(unit, err);
					});
			}
			else
				handler(NULL, static_cast<ReqError>(err));
			});
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
		ConfigFile* config = doCast<Rhea::ConfigFile>(m_unit_tree->getChild("config"));
		return config;
	}
}