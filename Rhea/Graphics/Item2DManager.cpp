#include "Event.h"

#include "Fabrics/Canvas/Item2D.h"

#include "Item2DManager.h"
#include "RenderElement.h"
#include "GraphicsService.h"

#include <Poseidon/SceneManager.h>
#include <Psio/Log.h>

namespace Rhea
{
	Element* GraphicsItem2DManager::createElement(Poseidon::Core* core, Unit* unit)
	{
		// check if already created
		Element* el = unit->getElement("graphics");
		if (el)
			return el;

		// make sure we have a material
		Rhea::Material* material = static_cast<Item2D*>(unit)->getMaterial();
		if (!material) {
			LOG(LOG_ERROR) << unit->getPath() << ": Item2D has no material";
			return nullptr;
		}

		// create the element
		GraphicsRenderElement* elem = new GraphicsRenderElement(core);
		Poseidon::Node* entity = createRenderable(elem, core, unit, "HUD", false);

		// set the material
		setMaterial(core, material, entity);
		unit->addElement("graphics", elem);

		// set up an empty transform
		setPosition(unit, ((Item2D*)unit)->getTransform());
		entity->setTransform(((Item2D*)unit)->getTransform());

		// register handlers
		unit->registerEventHandler("delete", this, std::bind(&GraphicsItem2DManager::onItemDelete, this, std::placeholders::_1, std::placeholders::_2));
		unit->registerEventHandler("update", this, std::bind(&GraphicsItem2DManager::onUpdate, this, std::placeholders::_1, std::placeholders::_2));

		// add to my list
		m_elements.push_back(elem);

		// inital update
		setItem2D(static_cast<Item2D*>(unit), elem);

		return elem;
	}

	void GraphicsItem2DManager::setItem2D(Item2D* item, GraphicsRenderElement* elem)
	{
		float z = 0.0f;
		float m_x = (float)item->X();
		float m_y = (float)item->Y();
		float m_w = (float)item->W();
		float m_h = (float)item->H();

		// adjust to height of viewport (y is negative)
		m_y = m_service->getViewportHeight() - m_y;

		float m_u1 = 0.0f;
		float m_v1 = 0.0f;
		float m_u2 = 1.0f;
		float m_v2 = 1.0f;

		std::vector<Hippios::vertex_uv> vertices =
		{
			{ m_x, m_y - m_h, z, m_u1, m_v2 }, // bottom-left
			{ m_x, m_y, z, m_u1, m_v1 }, // top-left
			{ m_x + m_w, m_y, z, m_u2, m_v1 }, // top-right
			{ m_x + m_w, m_y - m_h, z, m_u2, m_v2 }  // bottom-right
		};
		std::vector<unsigned long> v_indices = { 0, 1, 2, 0, 2, 3 };

		elem->getMesh()->setVertices(sizeof(Hippios::vertex_uv), vertices.size(), vertices.data());
		elem->getMesh()->setIndices(sizeof(unsigned long), v_indices.size() / 3, v_indices.data());
	}

	bool GraphicsItem2DManager::onUpdate(Event* ev, Unit* unit)
	{
		printf("item2d updated\n");

		Item2D* t = static_cast<Item2D*>(unit);
		GraphicsRenderElement* elem = static_cast<GraphicsRenderElement*>(t->getElement("graphics"));

		setItem2D(t, elem);

		return false;
	}

	void GraphicsItem2DManager::onFrame()
	{
	}

	bool GraphicsItem2DManager::onItemDelete(Event* ev, Unit* unit)
	{
		//erase element from list
		Element* elem = unit->getElement("graphics");
		auto elem_pos = std::find(m_elements.begin(), m_elements.end(), elem);
		if (elem_pos != m_elements.end())
			m_elements.erase(elem_pos);
		unit->addElement("graphics", nullptr);
		// free element
		delete elem;

		return false;
	}
}