#include "Item.h"
#include "Application.h"
#include "Poseidon/Entity.h"
#include "Poseidon/SceneManager.h"
#include "UnitTree.h"
#include "Mesh.h"
#include "Graphics/RenderElement.h"
#include "Physics/Body.h"
#include <Psio/Log.h>

namespace Rhea
{
	void Item::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);

		m_values["isItem"] = true;

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

		m_render_component = std::make_unique<RenderComponent>(this);

		if (m_values.find("fabric") != m_values.end())
			m_render_component->hide();
	}

	Unit* Item::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Item(name);
		return ret;
	}
	
	std::vector<Unit::Export> Item::getReferences()
	{
		std::vector<Export> refs;
		if (m_values.find("mesh") != m_values.end()) {
			refs.push_back({ "mesh", "//meworla/mesh", m_values["mesh"] });
		}
		return refs;
	}

	void Item::ready()
	{
		Unit::ready();

		auto mesh_it = m_values.find("mesh");
		if (!m_mesh && mesh_it != m_values.end()) {			
			m_mesh = static_cast<Mesh*>(m_app->getUnitTree()->find(mesh_it->second, m_attributes["basepath"], false));
			if (m_mesh) {
				m_mesh->ready();
				m_render_component->setMesh(m_mesh);
			}
			else {
				LOG(LOG_ERROR) << "No mesh defined for item";
			}
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
		std::unordered_map<std::string, std::string> attr = m_attributes;
		std::unordered_map<std::string, Variant> values = m_values;
		auto fit = values.find("fabric");
		if (fit != values.end())
			values.erase(fit);

		new_item->Unit::setup(m_app, attr, values);

		// copy item values
		new_item->m_render_component = std::make_unique<RenderComponent>(new_item);
		new_item->m_mesh = m_mesh;
		new_item->m_render_component->setMesh(m_mesh);
		new_item->m_material = m_material;
		new_item->m_render_component->setMaterial(m_material); //TODO check!

		return new_item;
	}

	void Item::onFrame(float delta)
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