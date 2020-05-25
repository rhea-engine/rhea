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
#include "Terrain.h"
#include "TextResource.h"
#include "ImageResource.h"
#include "InputMap.h"
#include "Dispatcher.h"
#include "Controller/Controller.h"
#include "Controller/BasicController.h"
#include "Controller/SpectatorController.h"

#include "Graphics/GraphicsService.h"
#include "Platform/PlatformService.h"
#include "Physics/PhysicsManager.h"

#include "Application.h"

namespace Rhea
{
	void Application::init()
	{		
		// initialize graphics system
		GraphicsService* g_service = new GraphicsService(this);
		m_services["graphics"] = g_service;
		Service* psio = new PlatformService(this);
		g_service->setPlatform(psio);
		m_services["platform"] = psio;
		core = g_service->getEngine();
		Service* phy_mgr = new PhysicsManagerService(this);
		m_services["physics_mgr"] = phy_mgr;

//		core = new Poseidon::Core();
		m_fs = new Orion::Orion(m_appname);
		m_unit_tree = new UnitTree(this);

		// register core types
		m_unit_tree->registerUnitType("//meworla/realm", Realm::createType);
		m_unit_tree->registerUnitType("//meworla/item", Item::createType);
		m_unit_tree->registerUnitType("//meworla/mesh", Mesh::createType);
		m_unit_tree->registerUnitType("//meworla/wavefront-object", TextResource::createType);
		m_unit_tree->registerUnitType("//meworla/wavefront-material", TextResource::createType);
		m_unit_tree->registerUnitType("//meworla/texture", ImageResource::createType);
		m_unit_tree->registerUnitType("//meworla/controller", BasicController::createType);
		m_unit_tree->registerUnitType("//meworla/spectator", SpectatorController::createType);
		m_unit_tree->registerUnitType("//meworla/camera", Camera::createType);
		m_unit_tree->registerUnitType("//meworla/heightmap", ImageResource::createType);
		m_unit_tree->registerUnitType("//meworla/terrain", Terrain::createType);

		// add a default input map
		m_input_map = new InputMap();
		m_input_map->loadDefaultMap();
		m_dispatcher = new Dispatcher();
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
		Orion::ResourceRequest* req = m_fs->getResourceByRid(resourceId);
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
				onSceneReady(realm_unit, req->getResource(), handler);
			}
			else {
				log(Orion::ResourceRequest::getErrorMessage(err) + ": " + resourceId);
			}
			});	
	}

	void Application::onSceneReady(Unit* parent, Orion::Resource* res, std::function<void(Scene*, ReqError error)> handler)
	{
		// for now we assume a scene file
		std::unique_ptr<SceneLoader> parser = std::make_unique<SceneLoader>();

		// parse the file
		Scene *scene = parser->load(parent, res, this);
		scene->load([&](ReqError err) {
			if (err == ReqError::OK) {
				handler(scene, err);
			}
			else {
				handler(NULL, err);
			}
			});
	}

	void Application::onResourceFailed(Orion::ResourceRequest::Error reason)
	{
		printf("Resource loading failed\n");
	}

	bool Application::onInput(std::shared_ptr<Psio::InputEvent> evt)
	{
		// close on esc
		Psio::KeyInputEvent esc(Psio::InputEventId::KEY_PRESSED);
		esc.set(Psio::Key::CODE_ESC, 0, 0);

		if (evt->isEvent(&esc)) {
			return true;
		}
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
		float frame_slice = 1.0 / iterations_per_second;
		uint64_t last_ticks = psio->getTicks();

		uint64_t frame = 0;
		int physics_steps = 0;
		float accumulated_time = 0; // time accumulated time since last physical steps
		float idle_step;

		float fps = 0;
		uint64_t frames = 0;

		uint64_t fps_start = psio->getTicks();
		uint64_t fps_count = 0;
		float elapsed_fs = 0.0f;

		while (!ret) {

			// timings
			uint64_t current_ticks = psio->getTicks();
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
			physics_steps = std::floor(accumulated_time * iterations_per_second);
			accumulated_time -= physics_steps * frame_slice;
			//if (physics_steps > 5)
			//	printf("phys steps: %d ; acc: %f        \r\n", physics_steps, accumulated_time);

			if (onFrameBegin())
				ret = true;

			// check for window and input events
			while (evt = psio->pollInput()) {
				// always handle quit
				if (evt->getId() == Psio::InputEventId::WMGR_QUIT) {
					ret = true;
				}
				// input map ?
				iea = m_input_map->getActions(evt);
				if (iea.actions.size())
				{
					for (auto& act : iea.actions) {
						if (m_focused_controller)
							m_focused_controller->onInput(act, evt);
						m_dispatcher->dispatch(act, evt);
					}
				}
				// handle input directly
				if (onInput(evt))
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
			if (m_focused_controller) {
				Poseidon::Position *pos = m_focused_controller->getTarget()->getNode()->get_position();
				Poseidon::Vector3 p = pos->get_worldspace();
				printf("\rPosition: \t%f\t%f\t%f", p.x, p.y, p.z);
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
				// we have a resource

				// create unit with the given type
				Unit* unit = m_unit_tree->createUnitById(resource_id, type_name);

				if (!unit)
					return handler(NULL, ReqError::PATH_NOT_FOUND);

				// set the app
				unit->setup(this);

				// call load of unit
				// TODO: check if really is a resource
				((Resource*)unit)->load([&](ReqError err) {
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