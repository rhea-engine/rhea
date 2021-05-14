#include <exception>
#include <algorithm>

#include "Hippios/Engine.h"
#include "Poseidon/Core.h"
#include "Orion/Orion.h"
#include "Orion/ResourceStream.h"
#include "Orion/ResourceRequest.h"
#include "Poseidon/SceneManager.h"
#include "Psio/Psio.h"
#include "Psio/InputEvent.h"
#include "Psio/Keyboard.h"
#include "Importer.h"
#include "SceneLoader.h"
#include "Unit.h"
#include "UnitTree.h"
#include "Realm.h"
#include "Item.h"
#include "Scene.h"
#include "Mesh.h"
#include "Camera.h"
#include "Fabrics/Canvas/Item2D.h"
#include "Fabrics/Terrain/Terrain.h"
#include "TextResource.h"
#include "ImageResource.h"
#include "InputMap.h"
#include "Dispatcher.h"
#include "Controller/Controller.h"
#include "Controller/BasicController.h"
#include "Controller/SpectatorController.h"
#include "Fabrics/Character/Character.h"
#include "Fabrics/Character/KinematicCharacter.h"
#include "Fabrics/GUI/GUIController.h"
#include "Fabrics/GUI/GuiService.h"

#include "Graphics/GraphicsService.h"
#include "Platform/PlatformService.h"
#include "Physics/PhysicsManager.h"

#include "SimpleParser/Type.h"
#include "SimpleParser/Method.h"

#include "Application.h"
#include <Fabrics/Text/TextWidget.h>
#include <Fabrics/Text/BitmapFont.h>
#include <EventInput.h>

namespace Rhea
{
	void Application::init()
	{		
		// register core types
		SimpleParser::Type::registerType<Unit>("//core/types/unit");
		SimpleParser::Type::registerType<UnitTree>("//core/unittree");

		// initialize services
		SimpleParser::Type::registerType<Service>("//meworla/service");
		SimpleParser::Type::registerType<GraphicsService>("//meworla/graphicsservice");
		SimpleParser::Type::registerType<PlatformService>("//meworla/platformservice");
		SimpleParser::Type::registerType<PhysicsManagerService>("//meworla/physicsmanagerservice");

		GraphicsService* g_service = (GraphicsService*)SimpleParser::Type::create("GraphicsService", "//meworla/graphicsservice");
		g_service->setup(this);
		m_services["graphics"] = g_service;
		Service* psio = (PlatformService*)SimpleParser::Type::create("PlatformService", "//meworla/platformservice");
		psio->setup(this);
		g_service->setPlatform(psio);
		m_services["platform"] = psio;
		core = g_service->getEngine();
		Service* phy_mgr = (PhysicsManagerService*)SimpleParser::Type::create("PhysicsManagerService", "//meworla/physicsmanagerservice");
		phy_mgr->setup(this);
		m_services["physics_mgr"] = phy_mgr;

//		core = new Poseidon::Core();
		m_fs = new Orion::Orion(m_appname);
		m_unit_tree = (UnitTree*)SimpleParser::Type::create("/", "//core/unittree");
		m_unit_tree->setup(this);

		// register scene types
		SimpleParser::Type::registerType<Realm>("//meworla/realm");
		SimpleParser::Type::registerType<Scene>("//meworla/scene");
		SimpleParser::Type::registerType<Item>("//meworla/item");
		SimpleParser::Type::registerType<Mesh>("//meworla/mesh");
		SimpleParser::Type::registerType<TextResource>("//meworla/wavefront-object");
		SimpleParser::Type::registerType<TextResource>("//meworla/wavefront-material");
		SimpleParser::Type::registerType<ImageResource>("//meworla/texture");
		SimpleParser::Type::registerType<BasicController>("//meworla/controller");
		SimpleParser::Type::registerType<SpectatorController>("//meworla/spectator");
		SimpleParser::Type::registerType<Camera>("//meworla/camera");
		SimpleParser::Type::registerType<ImageResource>("//meworla/heightmap");
		SimpleParser::Type::registerType<Terrain>("//meworla/terrain");
		SimpleParser::Type::registerType<KinematicCharacter>("//meworla/character");
		SimpleParser::Type::registerType<TextResource>("//meworla/shader");
		SimpleParser::Type::registerType<Item2D>("//meworla/widget");
		SimpleParser::Type::registerType<TextWidget>("//meworla/textwidget");
		SimpleParser::Type::registerType<BitmapFont>("//meworla/bitmapfont");
		SimpleParser::Type::registerType<TextResource>("//meworla/fontinfo");
		GUIController::Register(this);

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

	void Application::setup()
	{		
		for (auto it = m_services.begin(); it != m_services.end(); ++it)
		{
			Service* s = it->second;
			s->onInit();			
		}
		//core->init();
	}

	void Application::setAppDir(char* path)
	{
		if (!m_fs) {
			printf("Error: SetAppDir called before App::init()\n");
			exit(-1);
		}
		m_fs->setAppDir(path);
	}

	void Application::loadScene(const std::string resourceId, std::function<void(Scene*, ReqError error)> handler)
	{
		loadResource(resourceId, "//meworla/scene", [&](Rhea::Unit* scene, ReqError r_err) {
			if (r_err == ReqError::OK) {
				scene->prepare([&](Rhea::Error* err) {
					log("scene loaded");
					scene->ready();
					log("scene ready");
					handler((Scene*)scene, r_err);
					});
			}
			else {
				log("loading of scene failed");
				handler(nullptr, r_err);
			}
			});
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
			PhysicsService* physics = static_cast<PhysicsService*>(m_services["physics"]);
			physics->toggleDebug();
		}

		// pressed mouse button
		Psio::MouseInputEvent btn(Psio::InputEventId::MOUSE_BTN_PRESSED);


		return false;
	}

	
	int Application::loop()
	{
		// start the loop
		//int ret = core->loop();
		int ret = 0;	
		Psio::Engine* psio = static_cast<PlatformService*>(m_services["platform"])->getEngine();
		std::shared_ptr<Psio::InputEvent> evt;
		ievt_actions iea;
		PhysicsService* physics = static_cast<PhysicsService*>(m_services["physics"]);
		if (!physics) {
			physics = static_cast<PhysicsManagerService*>(m_services["physics_mgr"])->getPhysicsService(PhysicsManagerService::Types::WORLD);
			physics->onInit();
			m_services["physics"] = physics;
		}

		int iterations_per_second = 60;
		float frame_slice = 1.0f / iterations_per_second;
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

#ifdef _DEBUG
		Unit* u = m_unit_tree->find("//rhea/shaders");
		auto mth = u->getType()->getMethod("getName");
		std::string s = static_cast<std::string>(mth->execute(u));
		log("shader folder: " + s);
		Variant v = std::string("new_shaders");
		Variant* vs[1];
		vs[0] = &v;
		//u->getType()->getMethod("setName")->execute(u, v);
		u->getType()->getMethod("setName")->execute(u, vs, 1);
		log("shader folder: " + static_cast<std::string>(mth->execute(u)));
#endif

		while (!ret) {

			// timings
			current_ticks = psio->getTicks();
			uint64_t elapsed_ticks = current_ticks - last_ticks;
			float elapsed_s = (float)elapsed_ticks / 1000000L;
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

			if (onFrameBegin())
				ret = true;

			// check for window and input events
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
				m_unit_tree->sendEvent("gui_input", &e);
				m_unit_tree->sendEvent("unhandled_input", &e);
				m_unit_tree->sendEvent("scene_input", &e);				

				// application input handling comes last
				if (onInput(&e, nullptr))
					ret = true;
			}

			// physics

			uint64_t physics_begin = psio->getTicks();
			static int max_physics_steps = 10;
			physics_steps = std::min(physics_steps, max_physics_steps);

			for (int i=0; i < physics_steps; i++)
			{
				physics->step(frame_slice);
			}			

			// idle
			uint64_t idle_begin = psio->getTicks();
			
			// idle(elapsed_s)

			// render
			for (auto it = m_services.begin(); it != m_services.end(); ++it) {
				if (it->second->onFrame())
					ret = true;
			}			

			frames++;

			if (onFrameEnd())
				ret = true;
		}

		return ret;
	}

	void Application::loadResource(const std::string resource_id, Orion::ResourceRequest::RequestHandler handler)
	{
		Orion::ResourceRequest* req = m_fs->getResourceByRid(resource_id);
		req->resolve([&](Orion::ResourceRequest* req, Orion::ResourceRequest::Error err) {
			if (err == Orion::ResourceRequest::Error::OK) {
				// if we have a realm 
				std::string realm_name = req->getRealmName();
				Unit* realm_unit = m_unit_tree->find(realm_name, false);
				if (!realm_unit) {
					// add the realm
					realm_unit = (Unit*)new Realm(req->getRealm(), realm_name);
					m_unit_tree->addChild(realm_unit);
				}
			}
			handler(req, err);			
			});
	}

	void Application::loadResource(const std::string resource_id, std::string type_name, std::function<void(Unit*, ReqError error)> handler)
	{
		loadResource(resource_id, [&](Orion::ResourceRequest* req, ReqError err) {
			if (req) {
				// we have found the resource

				// find the parent and create the folders if necessary
				std::string parentpath = req->getRealmName() + req->getParentPath();
				Unit* parent_unit = m_unit_tree->find(parentpath, true);
				if (!parent_unit)
					// should not happen
					return handler(NULL, ReqError::PATH_NOT_FOUND);

				// create unit with the given type
				Unit* unit = m_unit_tree->createUnit(req->getBasename(), type_name);
				if (!unit)
					// should not happen
					return handler(NULL, ReqError::PATH_NOT_FOUND);

				// add it to tree
				parent_unit->addChild(unit);

				// call load of unit				
				// TODO: check if really is a resource
				((Resource*)unit)->load(req, [&](ReqError err) {
					handler(unit, err);
					});
			}
			else
				handler(NULL, err);
			});
	}
	
	void Application::log(std::string message)
	{
		printf("%s\n", message.c_str());
	}

	void Application::close()
	{

	}
}