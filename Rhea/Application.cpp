#include <exception>

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
#include "TextResource.h"
#include "ImageResource.h"
#include "InputMap.h"
#include "Dispatcher.h"

#include "Application.h"

namespace Rhea
{
	void Application::init()
	{		
		core = new Poseidon::Core();
		m_fs = new Orion::Orion(m_appname);
		m_unit_tree = new UnitTree(this);

		// register core types
		m_unit_tree->registerUnitType("//meworla/realm", Realm::createType);
		m_unit_tree->registerUnitType("//meworla/item", Item::createType);
		m_unit_tree->registerUnitType("//meworla/mesh", Mesh::createType);
		m_unit_tree->registerUnitType("//meworla/wavefront-object", TextResource::createType);
		m_unit_tree->registerUnitType("//meworla/wavefront-material", TextResource::createType);
		m_unit_tree->registerUnitType("//meworla/texture", ImageResource::createType);		

		// add a default input map
		m_input_map = new InputMap();
		m_input_map->loadDefaultMap();
		m_dispatcher = new Dispatcher();
	}

	void Application::setup()
	{
		core->init();
	}

	void Application::setAppDir(char* path)
	{
		if (!m_fs) {
			printf("Error: SetAppDir called before App::init()\n");
			exit(-1);
		}
		m_fs->setAppDir(path);
	}

	void Application::loadScene(const std::string resourceId)
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
				onSceneReady(realm_unit, req->getResource());
				m_unit_tree->logMe();
			}
			else {
				log(Orion::ResourceRequest::getErrorMessage(err) + ": " + resourceId);
			}
			});	
	}

	void Application::onSceneReady(Unit* parent, Orion::Resource* res)
	{
		// for now we assume a scene file
		std::unique_ptr<SceneLoader> parser = std::make_unique<SceneLoader>();

		// parse the file
		Scene *scene = parser->load(parent, res, this);
		scene->load([&](ReqError err) {
			if (err == ReqError::OK) {
				log("scene ready");
				scene->show(core);
			}
			else {
				log("Could not open scene");
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
		Psio::Engine* psio = core->getPsio();
		std::shared_ptr<Psio::InputEvent> evt;
		ievt_actions iea;

		while (!ret) {

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
					for (auto& act : iea.actions)
						m_dispatcher->dispatch(act, evt);
				}
				// handle input directly
				if (onInput(evt))
					ret = true;
			}

			if (core->iterate())
				ret = true;

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