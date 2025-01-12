#include "Item.h"
#include "Application.h"
#include "Poseidon/Entity.h"
#include "Poseidon/SceneManager.h"
#include "UnitTree.h"
#include "../3D/Mesh.h"
#include "Graphics/RenderElement.h"
#include "Physics/Body.h"
#include <Psio/Log.h>
#include <Math/Math.h>

namespace Rhea
{
	void Item::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);
		
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
		Variant rot_sit = getValue("rotation");
		if (rot_sit != Variant() && rot_sit.size()==3) {
			float x = rot_sit[0];
			float y = rot_sit[1];
			float z = rot_sit[2];
			Math::Quaternion rot;
			if (x != 0.0f)
				rot.fromAngleAxis(x * Math::M_DEGTORAD, Math::Vector3(1.0f, 0.0f, 0.0f));
			if (y != 0.0f)
				rot.fromAngleAxis(y * Math::M_DEGTORAD, Math::Vector3(0.0f, 1.0f, 0.0f));
			if (z != 0.0f)
				rot.fromAngleAxis(z * Math::M_DEGTORAD, Math::Vector3(0.0f, 0.0f, 1.0f));
			m_transform.get_position()->setOrientation(rot);
		}
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

		RenderComponent* render_component  = getComponent<RenderComponent>();

		auto mesh_it = m_values.find("mesh");
		if (mesh_it != m_values.end()) {
			if (!render_component) {
				render_component = addComponentUnique<RenderComponent>("renderer", "//meworla/rendercomponent");				
			}
			Mesh* mesh = doCast<Mesh>(m_app->getUnitTree()->find(mesh_it->second, m_attributes["basepath"], false));
			if (mesh) {
				mesh->ready();
				render_component->setMesh(mesh);
			}
			else {
				LOG(LOG_ERROR) << "No mesh defined for item";
			}
		}	

		if (render_component && m_values.find("fabric") != m_values.end())
			render_component->hide();		
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

		// copy item values and components
		if (getComponent<RenderComponent>()) {
			RenderComponent* render_component = new_item->addComponentUnique<RenderComponent>("renderer", "//meworla/rendercomponent");
			render_component->setMesh(render_component->getMesh());
		}

		return new_item;
	}

	void Item::onFrame(float delta)
	{
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