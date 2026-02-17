#include <Poseidon/Entity.h>
#include <Poseidon/SceneManager.h>
#include <Poseidon/Material.h>
#include <Poseidon/Shader.h>
#include <Poseidon/RenderContext.h>

#include <Rhea/Application.h>
#include <Rhea/Unit.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Fabrics/3D/Mesh.h>
#include <Rhea/Core/Material.h>
#include <Rhea/Resources/TextResource.h>
#include <Rhea/UnitTree.h>

#include "ItemManager.h"
#include "Texture.h"
#include "MeshElement.h"
#include "GraphicsService.h"

#include <Psio/Log.h>
#include <Rhea/Fabrics/3D/Light.h>
#include "GraphicsComponent.h"
#include "MaterialManager.h"
#include <Rhea/Components/RenderComponent.h>

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

		DLOG(LOG_DEBUG) << "GraphicsItemManager::createElement called for: " << item->getPathName();

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
		
		auto* rc = item->getComponent<RenderComponent>();
		Mesh* item_mesh = nullptr;
		if (!elem)
			elem = new GraphicsRenderElement(core);
		if (!rc || !rc->mesh)
		{
			VisualComponent* v_c = item->getComponent<VisualComponent>();
			if (!v_c) {
				DLOG(LOG_DEBUG) << "Item has no visual component";
				return nullptr;
				//v_c = item->addComponentUnique<VisualComponent>("visual");
			}

			Application* app = m_service->getApplication();
			if (!app) {
				LOG(LOG_ERROR) << "Graphics service has no app reference";
				return nullptr;
			}
			item_mesh = doCast<Mesh>(app->getUnitTree()->find(v_c->meshRef(), v_c->basepathRef(), false));
			if (item_mesh) {
				item_mesh->ready();
				if (!rc)
					rc = item->addComponentUnique<RenderComponent>("renderer");
				rc->setMesh(item_mesh);
			}
			else {
				LOG(LOG_ERROR) << "No mesh defined for item";
				return nullptr;
			}			
		}
		else
			item_mesh = rc->mesh;

		if (item_mesh) {

			// check if mesh element already defined
			if (!item_mesh->getComponent<GraphicsComponent>()) {
				// create using mesh manager
				GraphicsMesh* mesh_el = (GraphicsMesh*)m_service->getManager<Mesh>()->createElement(core, item_mesh);
			}

			// create the element		
			Poseidon::Renderable* entity = static_cast<Poseidon::Renderable*>(createRenderable(elem, core, unit, "default", item_mesh->isDynamic()));
		}
		else if (light) {
			Poseidon::Renderable* entity = static_cast<Poseidon::Renderable*>(createEntity(elem, core, unit, "default", false));
		}
		// NOTE else do nothing: this is normal for widgets that have no or transparent background defined

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
		auto* rc = item->getComponent<RenderComponent>();
		if (!rc || !rc->mesh) return false;
		Mesh* mesh = rc->mesh;

		// NOTE: This assumes a changed material will actually be a new material (i.e. the old one gets destroyed)

		// check if mesh has already a component, if not create it
		if (!mesh->getComponent<GraphicsComponent>()) {
			// create using mesh manager
			GraphicsMesh* mesh_el = (GraphicsMesh*)m_service->getManager<Mesh>()->createElement(m_service->getEngine(), mesh);
		}
		// ensure all submeshes have the correct material
		GraphicsMesh* mesh_el = static_cast<GraphicsMesh*>(mesh->getComponent<GraphicsComponent>()->getElement());
		for (size_t i = 0; i < mesh->getNumSubmeshes(); i++) {
			Material* material = mesh->getMaterial(i);
			if (!material) {
				LOG(LOG_ERROR) << "GraphicsMeshManager::createElement: No material for submesh " << i;
				return false;
			}
			GraphicsComponent* mat_gr_comp = material->getComponent<GraphicsComponent>();
			if (!mat_gr_comp) {
				if (!m_service->addElement(material)) {
					LOG(LOG_ERROR) << "GraphicsItemManager::createElement: could not create element for material";
					return false;
				}
				else
					mat_gr_comp = material->getComponent<GraphicsComponent>();
			}
			// set the material in core
			setMaterial(m_service->getEngine(), material);
			// set the poseidon material
			if (mesh_el && mesh_el->m_mesh) {
				Poseidon::Mesh::SubMesh* submesh = mesh_el->m_mesh->getSubMesh(i);
				if (submesh) {
					GraphicsMaterial* gr_mat = static_cast<GraphicsMaterial*>(mat_gr_comp->getElement());
					if (gr_mat)
						submesh->material = gr_mat->m_material;
				}
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
