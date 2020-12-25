#include "Item.h"
#include "Application.h"
#include "Poseidon/Entity.h"
#include "Poseidon/SceneManager.h"
#include "UnitTree.h"
#include "Mesh.h"
#include "Graphics/Item.h"
#include "Physics/Body.h"
#include "ResourceList.h"

namespace Rhea
{
	void Item::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);
		m_status = Status::SETUP;

		m_resource_list = new ResourceList(app);

		// dependent resources
		if (m_values.find("mesh") != m_values.end())
			m_resource_list->push("mesh", getPath((std::string)m_values["mesh"]), "//meworla/mesh");
	}

	Unit* Item::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Item(name);
		return ret;
	}
	
	void Item::ready()
	{
		if (m_status >= Status::READY)
			return;
		m_status = Status::READY;
		Unit::ready();

		if (!m_mesh) {
			m_mesh = static_cast<Mesh*>(m_app->getUnitTree()->find(getPath((std::string)m_values["mesh"]), false));
			if (m_mesh)
				m_mesh->ready();
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