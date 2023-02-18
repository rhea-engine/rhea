#include "Item.h"
#include "Item.h"
#include "Application.h"
#include "Poseidon/Entity.h"
#include "Poseidon/SceneManager.h"
#include "UnitTree.h"
#include "Mesh.h"
#include "Graphics/RenderElement.h"
#include "Physics/Body.h"
#include "ResourceList.h"
#include <Psio/Log.h>

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

		Variant pos_sit = getValue("position");
		if (pos_sit != Variant()) {
			float x = pos_sit[0];
			float y = pos_sit[1];
			float z = 0.0f;
			if (pos_sit.size() > 2) {
				z = pos_sit[2];
			}
			m_transform.get_position()->setPosition(x, y, z);
		}
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

		if (!m_mesh && m_resource_list) {
			m_mesh = static_cast<Mesh*>((*m_resource_list)["mesh"]);
			if (m_mesh)
				m_mesh->ready();
		}
	}

	bool Item::getGraphicsItem()
	{
		auto it = m_elements.find("graphics");
		if (it != m_elements.end()) {
			m_graphics_item = static_cast<GraphicsRenderElement*>(it->second);
			return true;
		}
		return false;
	}

	Item* Item::instance(std::string name)
	{
		Item* new_item = (Rhea::Item*)SimpleParser::Type::create(name, getTypeName());
		if (!new_item) {
			LOG(LOG_ERROR) << "could not instance type with type " << getTypeName();
			return nullptr;
		}
		std::unordered_map<std::string, std::string> attr;
		new_item->Unit::setup(m_app, attr, m_values);
		new_item->m_status = Status::SETUP;

		// copy item values
		new_item->m_mesh = m_mesh;
		new_item->m_material = m_material;

		return new_item;
	}

	void Item::update()
	{
		// update myself
		m_transform.update();		
	}

	void Item::setPosition(Rhea::Position& position)
	{
		m_transform.get_position()->setPosition(position);
		m_transform.update();
	}
	
	void Item::setPosition(Math::Vector3 position)
	{
		m_transform.get_position()->setPosition(position);
		m_transform.update();
	}

	void Item::setLookAt(Math::Vector3 look_at)
	{
		m_transform.get_position()->lookAt(look_at);
		m_transform.update();
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
		return nullptr;
	}

	void Item::setMovement(Math::Vector3 move)
	{
		m_movement = move;
		if (m_net_component)
			m_net_component->setMovement(move);
		if (!m_physics_body)
			if (!getPhysicsBody())
				return;
		m_physics_body->move(move);
	}

	void Item::setTurn(float turn)
	{
		m_turn = turn;
		if (m_net_component)
			m_net_component->setTurn(turn);
		if (!m_physics_body)
			if (!getPhysicsBody())
				return;
		m_physics_body->turn(turn);
	}	
}