#include "Item.h"
#include "Application.h"
#include "Poseidon/Entity.h"
#include "Poseidon/SceneManager.h"
#include "UnitTree.h"
#include "Mesh.h"
#include "Graphics/Item.h"
#include "Physics/Body.h"

namespace Rhea
{
	void Item::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
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
			std::string val = m->second;
			if (val.substr(0, 1) == "/")
				path = m->second;
			else if (m_parent)
				path = m_parent->getPath() + "/" + val;
			else
				path = "//" + val;

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

	bool Item::getGraphicsItem()
	{
		auto it = m_elements.find("graphics");
		if (it != m_elements.end()) {
			m_graphics_item = static_cast<GraphicsItem*>(it->second);
			return true;
		}
		return false;
	}

	void Item::setPosition(Poseidon::Position& position)
	{
		if (!m_graphics_item)
			if (!getGraphicsItem())
				return;
		m_graphics_item->setPosition(position);
	}
	
	void Item::setPosition(Poseidon::Vector3 position)
	{
		if (!m_graphics_item)
			if (!getGraphicsItem())
				return;
		m_graphics_item->setPosition(position);
	}

	void Item::setLookAt(Poseidon::Vector3 look_at)
	{
		if (!m_graphics_item)
			if (!getGraphicsItem())
				return;
		m_graphics_item->lookAt(look_at);
	}
	
	PhysicsBody* Item::getPhysicsBody()
	{
		if (m_physics_body)
			return m_physics_body;

		auto it = m_elements.find("physics");
		if (it != m_elements.end()) {
			m_physics_body = static_cast<PhysicsBody*>(it->second);
			return m_physics_body;
		}
		//m_app->log(m_pathname + ": getPhysicsBody: not found");
		return nullptr;
	}

	void Item::setMovement(Poseidon::Vector3 move)
	{
		m_movement = move;
		if (!m_physics_body)
			if (!getPhysicsBody())
				return;
		m_physics_body->move(move);
	}

	void Item::setTurn(float turn)
	{
		m_turn = turn;
		if (!m_physics_body)
			if (!getPhysicsBody())
				return;
		m_physics_body->turn(turn);
	}

	void Item::addFollower(Item* item)
	{
		// simply add as child to entity
		Poseidon::Node* child = item->getNode();
		child->setCoordSpace(1);
		if (!m_graphics_item)
			if (!getGraphicsItem())
				return;
		m_graphics_item->getEntity()->addChild(child);
	}
	
	Poseidon::Node* Item::getNode()
	{
		if (!m_graphics_item)
			if (!getGraphicsItem())
				return nullptr;
		return m_graphics_item->getEntity();
	}
}