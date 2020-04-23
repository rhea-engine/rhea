#include "Item.h"
#include "Application.h"
#include "Poseidon/Entity.h"
#include "Poseidon/SceneManager.h"
#include "UnitTree.h"
#include "Mesh.h"

namespace Rhea
{
	void Item::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, std::string> values)
	{
		m_attributes = attributes;
		m_values = values;
		m_app = app;
		m_status = Status::SETUP;
	}

	void Item::load(ReadyHandler handler)
	{
		if (m_status >= Status::LOADED)
			return handler(ReqError::OK);

		if (m_status == Status::SETUP) {
			loadDependencies(handler);
		}
		else {
			// TODO load myself and then call dependencies
			printf("Item::load without setup not implemented\n");
		}

	}

	void Item::show(Poseidon::Core* core)
	{
		Poseidon::SceneManager* scene_mgr = core->get_scene_manager();

		Poseidon::Mesh* engine_mesh = m_mesh->getMesh();
		m_entity = core->createEntity(engine_mesh);
		scene_mgr->getViewport()->addRenderChild(m_entity);

		Poseidon::Position* pos = m_entity->get_position();
		pos->setPosition(0.0f, 0.0f, 4.0f);
	}

	Unit* Item::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Item(name);
		return ret;
	}
	
	void Item::loadDependencies(ReadyHandler handler)
	{
		// get all resources we depend on
		auto m = m_values.find("mesh");
		if (m != m_values.end()) {
			std::string path;
			if (m->second.substr(0, 1) == "/")
				path = m->second;
			else if (m_parent)
				path = m_parent->getPath() + "/" + m->second;
			else
				path = "//" + m->second;

			// check if resource already loaded
			Unit* unit = m_app->getUnitTree()->find(path, false);
			if (unit) {
				std::string type = unit->getType();
				if (type == "//meworla/mesh") {
					m_mesh = (Mesh*)unit;
					m_mesh->load([&](ReqError err) {
						m_status = Status::LOADED;
						handler(err);
						});
					return;
				}
				else {
					m_status = Status::LOADED;
					handler(ReqError::OK);
					return;
				}
			}

			m_app->loadResource(path, [&](Orion::ResourceRequest* req, Orion::ResourceRequest::Error err) {
				
				if (req) {
					m_status = Status::LOADED;
					// TODO: m_mesh = 
					handler(ReqError::OK);
				}
				else {
					m_status = Status::NOT_FOUND;
					m_app->log(Orion::ResourceRequest::getErrorMessage(err) + ": " + path);
					handler(err);
				}
				});
		}
		else {
			m_status = Status::LOADED;
			handler(ReqError::OK);
		}
	}
}