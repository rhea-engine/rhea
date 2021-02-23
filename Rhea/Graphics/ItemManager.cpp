#include <Poseidon/Entity.h>
#include <Poseidon/SceneManager.h>
#include <Poseidon/Material.h>
#include <Poseidon/Shader.h>
#include <Poseidon/RenderContext.h>

#include "../Unit.h"
#include "../Item.h"
#include "../Mesh.h"
#include "../Material.h"
#include "../TextResource.h"
#include "../Application.h"
#include "../UnitTree.h"

#include "ItemManager.h"
#include "Mesh.h"
#include "Texture.h"
#include "MeshElement.h"

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

		// check if mesh already defined
		GraphicsMesh* mesh_el = static_cast<GraphicsMesh*>(item_mesh->getElement("graphics"));
		if (mesh_el) {
			// reference the existing mesh
			elem->SetMesh(mesh_el->m_mesh);
		}

		// create the element		
		Poseidon::Node* entity = createRenderable(elem, core, unit, "default");
		
		if (!mesh_el) {
			// mesh was not defined up till now - store it in mesh unit

			// create mesh element
			setItem(item, elem);
			mesh_el = new GraphicsMesh();
			mesh_el->m_mesh = elem->getMesh();

			// attach it to mesh resource
			item_mesh->addElement("graphics", mesh_el);
		}

		// add the position
		addPosition(unit, entity);

		Rhea::Material* material = item_mesh->getMaterial();
		if (!material) {
			unit->getApplication()->log("Item has no material");
			delete elem;
			return nullptr;
		}
		setMaterial(core, material, entity);

		// attach to unit
		unit->addElement("graphics", elem);

		// register handlers
		unit->registerEventHandler("delete", std::bind(&GraphicsItemManager::onItemDelete, this, std::placeholders::_1, std::placeholders::_2));
		unit->registerEventHandler("update", std::bind(&GraphicsItemManager::onUpdate, this, std::placeholders::_1, std::placeholders::_2));

		// add to my list
		m_elements.push_back(elem);

		return elem;
	}

	void GraphicsItemManager::setItem(Item* item, GraphicsRenderElement* elem)
	{
		Poseidon::Mesh* m_mesh = elem->getMesh();
		Mesh* mesh = item->getMesh();

		// add vertex and index buffer

		std::vector<Mesh::vertex> vertices;
		std::vector<unsigned long> v_indices;

		mesh->getBuffers(vertices, v_indices);

		if (!vertices.size() || !v_indices.size()) {
			printf("Vertex is empty\n");
			return;
		}

		std::vector<Hippios::vertex_uv> hippios_vertices;
		for (auto it = vertices.begin(); it != vertices.end(); ++it) {
			Mesh::vertex v = *it;
			hippios_vertices.push_back(Hippios::vertex_uv(v.x, v.y, v.z, v.u, v.v));
		}

		m_mesh->setVertices(sizeof(Hippios::vertex_uv), hippios_vertices.size(), hippios_vertices.data());
		m_mesh->setIndices(sizeof(unsigned long), v_indices.size() / 3, v_indices.data());		
	}

	bool GraphicsItemManager::onUpdate(Event* ev, Unit* unit)
	{
		printf("item updated\n");

		Item* item = static_cast<Item*>(unit);
		GraphicsRenderElement* elem = static_cast<GraphicsRenderElement*>(item->getElement("graphics"));

		setItem(item, elem); // TODO: what could possibly be an update?

		return false;
	}

	bool GraphicsItemManager::onItemDelete(Event* ev, Unit* unit)
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
