#include <Poseidon/Entity.h>
#include <Poseidon/SceneManager.h>

#include "../Unit.h"
#include "../Item.h"
#include "../Mesh.h"

#include "Item.h"
#include "Mesh.h"

namespace Rhea
{
	GraphicsItem::~GraphicsItem()
	{
		m_core
			->get_scene_manager()
			->getViewport()
			->removeRenderChild(static_cast<Poseidon::Entity*>(m_entity));
		delete m_entity;
	}

	void GraphicsItem::createItem(Item* item)
	{
		Poseidon::SceneManager* scene_mgr = m_core->get_scene_manager();

		// create mesh element
		Mesh* mesh = item->getMesh();
		if (!mesh)
			return;
		GraphicsMesh* el = static_cast<GraphicsMesh*>(GraphicsMesh::createElement(m_core, mesh));

		Poseidon::Mesh* engine_mesh = el->getMesh();
		m_entity = m_core->createEntity(engine_mesh);
		scene_mgr->getViewport()->addRenderChild(static_cast<Poseidon::Entity*>(m_entity));

		Poseidon::Position* pos = m_entity->get_position();

		Variant pos_sit = item->getValue("position");
		if (pos_sit != Variant()) {
			float x = pos_sit[0];
			float y = pos_sit[1];
			float z = pos_sit[2];
			pos->setPosition(x, y, z);
		}
		else
			pos->setPosition(0.0f, 0.0f, 0.0f);

		item->addElement("graphics", this);
	}

	void GraphicsItem::setPosition(Poseidon::Position& position)
	{
		m_entity->get_position()->setPosition(position);
	}

	void GraphicsItem::setPosition(Poseidon::Vector3 position)
	{		
		m_entity->get_position()->setPosition(position);
	}

	void GraphicsItem::lookAt(Poseidon::Vector3 look_at)
	{
		m_entity->get_position()->lookAt(look_at);
	}

	Element* GraphicsItem::createElement(Poseidon::Core* core, Unit* unit)
	{
		// check if already created
		Element* el = unit->getElement("graphics");
		if (el)
			return el;

		GraphicsItem* item = new GraphicsItem(core);
		item->createItem(static_cast<Item*>(unit));

		return item;
	}
}
