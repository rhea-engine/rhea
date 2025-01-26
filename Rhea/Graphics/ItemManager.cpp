#include <Poseidon/Entity.h>
#include <Poseidon/SceneManager.h>
#include <Poseidon/Material.h>
#include <Poseidon/Shader.h>
#include <Poseidon/RenderContext.h>

#include "../Unit.h"
#include "Fabrics/Core/Item.h"
#include "Fabrics/3D/Mesh.h"
#include "Fabrics/Core/Material.h"
#include "../Resources/TextResource.h"
#include "../UnitTree.h"

#include "ItemManager.h"
#include "Texture.h"
#include "MeshElement.h"
#include "GraphicsService.h"

#include <Psio/Log.h>
#include <Rhea/Fabrics/3D/Light.h>
#include "GraphicsComponent.h"

namespace Rhea
{
	GraphicsItemManager::~GraphicsItemManager()
	{
	
	}

	GraphicsElement* GraphicsItemManager::createElement(Poseidon::Core* core, Unit* unit)
	{
		GraphicsRenderElement* elem = nullptr;

		// check if already created
		GraphicsComponent* item_el = unit->getComponent<GraphicsComponent>();
		if (item_el)
			return item_el->getElement();

		Item* item = doCast<Item>(unit);
		if (!item)
			return nullptr;

		// is this a light?
		Light* light = doCast<Light>(item);
		if (light) {			
			DLOG(LOG_DEBUG) << "GraphicsItemManager::createElement: light";
			elem = new GraphicsRenderElement(core);
			Poseidon::Renderable* light_entity = static_cast<Poseidon::Renderable*>(createEntity(elem, core, unit, "default", false));
			// attach to unit
			unit->addComponentUnique<GraphicsComponent>(elem);
		}

		// prepare basic elements
		
		Mesh* item_mesh = item->getMesh();
		if (!light && !item_mesh) {
			DLOG(LOG_DEBUG) << "GraphicsItemManager::createElement: getMesh returned nullptr";	
			return nullptr;
		}
		elem = new GraphicsRenderElement(core);

		if (item_mesh) {

			// check if mesh element already defined
			if (!item_mesh->getComponent<GraphicsComponent>()) {
				// create using mesh manager
				GraphicsMesh* mesh_el = (GraphicsMesh*)m_service->getManager<Mesh>()->createElement(core, item_mesh);
			}

			// create the element		
			Poseidon::Renderable* entity = static_cast<Poseidon::Renderable*>(createRenderable(elem, core, unit, "default", item_mesh->isDynamic()));
		}
		else {
			Poseidon::Renderable* entity = static_cast<Poseidon::Renderable*>(createEntity(elem, core, unit, "default", false));
		}

		// attach to unit
		unit->addComponentUnique<GraphicsComponent>(elem);

		// register handlers
		unit->registerEventHandler("delete", this, std::bind(&GraphicsItemManager::onItemDelete, this, std::placeholders::_1, std::placeholders::_2));
		unit->registerEventHandler("update", this, std::bind(&GraphicsItemManager::onUpdate, this, std::placeholders::_1, std::placeholders::_2));

		// add to my list
		m_elements.push_back(elem);

		return elem;
	}

	bool GraphicsItemManager::onUpdate(Event* ev, Unit* unit)
	{
		Item* item = static_cast<Item*>(unit);
		GraphicsComponent* gr_comp = item->getComponent<GraphicsComponent>();
		if (!gr_comp)
			return nullptr;
		GraphicsRenderElement* elem = static_cast<GraphicsRenderElement*>(gr_comp->getElement());

		// possible material change - check if material has element
		//Mesh* mesh = item->getGraphicsComponent()->getMesh();
		Mesh* mesh = item->getMesh();
		if (!mesh)
			return false;

		if (Material* mat = mesh->getMaterial()) {
			if (!mat->getComponent<GraphicsComponent>()) {
				// create the element for the new material
				GraphicsManager* mgr = m_service->getManager(mat->getType());
				mgr->createElement(m_service->getEngine(), mat);
			}
		}

		return false;
	}

	bool GraphicsItemManager::onItemDelete(Event* ev, Unit* unit)
	{
		//erase element from list
		GraphicsComponent* g_component = unit->getComponent<GraphicsComponent>();
		if (g_component) {
			GraphicsRenderElement* elem = (GraphicsRenderElement*)(g_component->getElement());
			auto elem_pos = std::find(m_elements.begin(), m_elements.end(), elem);
			if (elem_pos != m_elements.end())
				m_elements.erase(elem_pos);
		}
		return false;
	}
}
