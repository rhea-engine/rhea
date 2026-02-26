#include "Rhea/Core/Item.h"
#include "Rhea/Application.h"
#include "Rhea/UnitTree.h"
#include <Rhea/Fabrics/3D/Mesh.h>
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

		VisualComponent* visual_component  = getComponent<VisualComponent>();

		auto mesh_it = m_values.find("mesh");
		if (mesh_it != m_values.end()) {
			if (!visual_component) {
				visual_component = addComponentUnique<VisualComponent>("visual");
			}
			// only set ref
			visual_component->setMeshRef(mesh_it->second, m_attributes["basepath"]);
/*			Mesh* mesh = doCast<Mesh>(m_app->getUnitTree()->find(mesh_it->second, m_attributes["basepath"], false));
			if (mesh) {
				mesh->ready();
				render_component->setMesh(mesh);
			}
			else {
				LOG(LOG_ERROR) << "No mesh defined for item";
			}
			*/
		}			

		if (visual_component && m_values.find("fabric") != m_values.end())
			visual_component->hide();
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
		if (getComponent<VisualComponent>()) {

			// TODO: copy all components using their own copy function?
			// 			RenderComponent* render_component = new_item->addComponentUnique<RenderComponent>("renderer");
			// 			render_component->setMesh(render_component->getMesh());
			VisualComponent* v_c = getComponent<VisualComponent>();
			VisualComponent* visual_component = new_item->addComponentUnique<VisualComponent>("visual");
			visual_component->setMeshRef(v_c->meshRef(), v_c->basepathRef());
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
		
	void Item::setMovement(Math::Vector3 move)
	{
		m_movement = move;
		for (auto* l : m_movement_listeners) {
			l->onMovementChanged(move);
		}		
	}	

	void Item::setTurn(float turn)
	{
		m_turn = turn;
		for (auto* l : m_movement_listeners) {
			l->onTurnChanged(turn);
		}
	}	

	void Item::addComponent(Component* c)
	{
		Unit::addComponent(c);
		if (auto* l = doCast<MovementListenerComponent>(c))
			m_movement_listeners.push_back(l);
	}

	void Item::removeComponent(Component* c)
	{
		if (auto* l = doCast<MovementListenerComponent>(c)) {
			auto it = std::remove(m_movement_listeners.begin(), m_movement_listeners.end(), l);
			m_movement_listeners.erase(it, m_movement_listeners.end());
		}
		Unit::removeComponent(c);
	}
}