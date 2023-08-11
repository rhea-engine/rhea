#include <Poseidon/Entity.h>
#include <Poseidon/SceneManager.h>
#include <Poseidon/Material.h>
#include <Poseidon/Shader.h>
#include <Poseidon/RenderContext.h>

#include "../Unit.h"
#include "Fabrics/3D/Item.h"
#include "Fabrics/3D/Mesh.h"
#include "Fabrics/Core/Material.h"
#include "../Resources/TextResource.h"
#include "../UnitTree.h"

#include "ItemManager.h"
#include "Texture.h"
#include "MeshElement.h"
#include "GraphicsService.h"

#include <Psio/Log.h>

namespace Rhea
{
	Element* GraphicsItemManager::createElement(Poseidon::Core* core, Unit* unit)	
	{
		// check if already created
		Element* item_el = unit->getElement("graphics");
		if (item_el)
			return item_el;

		// prepare basic elements
		GraphicsRenderElement* elem = new GraphicsRenderElement(core);
		Item* item = static_cast<Item*>(unit);
		Mesh* item_mesh = item->getMesh();

		// safety check
		if (!item_mesh) {
			DLOG(LOG_DEBUG) << "GraphicsItemManager::createElement: getMesh returned nullptr";
			return nullptr;
		}
		
		// make sure we have a material
		Rhea::Material* material = item_mesh->getMaterial();
		if (!material) {
			LOG(LOG_ERROR) << unit->getPath() + ": item has no material";
			delete elem;
			return nullptr;
		}
		if (!material->getElement("graphics")) {
			if (!m_service->addElement(material)) {
				LOG(LOG_ERROR) << "GraphicsItemManager::createElement: could not create element for material";
				delete elem;
				return nullptr;
			}
		}

		// check if mesh element already defined
		GraphicsMesh* mesh_el = static_cast<GraphicsMesh*>(item_mesh->getElement("graphics"));
		if (!mesh_el) {
			// create using mesh manager
			mesh_el = (GraphicsMesh*)m_service->getManager("//meworla/mesh")->createElement(core, item_mesh);
		}

		// create the element		
		Poseidon::Renderable* entity = static_cast<Poseidon::Renderable*>(createRenderable(elem, core, unit, "default", item_mesh->isDynamic()));

		// set the material
		setMaterial(core, material);

		// attach to unit
		unit->addElement("graphics", elem);

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
		GraphicsRenderElement* elem = static_cast<GraphicsRenderElement*>(item->getElement("graphics"));

		// possible material change - check if material has element
		if (Material* mat = item->getGraphicsComponent()->getMaterial()) {
			if (!mat->getElement("graphics")) {
				// create the element for the new material
				GraphicsManager* mgr = m_service->getManager(mat->getTypeName());
				mgr->createElement(m_service->getEngine(), mat);
			}
		}

		return false;
	}

	bool GraphicsItemManager::onItemDelete(Event* ev, Unit* unit)
	{
		//erase element from list
		GraphicsRenderElement* elem = (GraphicsRenderElement*)(unit->getElement("graphics"));
		auto elem_pos = std::find(m_elements.begin(), m_elements.end(), elem);
		if (elem_pos != m_elements.end())
			m_elements.erase(elem_pos);

		// free element
		unit->addElement("graphics", nullptr);

		return false;
	}
}
