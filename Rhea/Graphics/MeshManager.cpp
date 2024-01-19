#include "MeshManager.h"
#include <Rhea/Fabrics/3D/Item.h>
#include <Rhea/Fabrics/3D/Mesh.h>
#include <Poseidon/Mesh.h>
#include <Poseidon/SceneManager.h>
#include <Psio/Log.h>
#include <Rhea/Fabrics/Core/Material.h>
#include "GraphicsService.h"

namespace Rhea
{
	Element* GraphicsMeshManager::createElement(Poseidon::Core* core, Unit* unit)
	{
		// check if already created
		Element* el = unit->getElement("graphics");
		if (el)
			return el;
		
		// create the element
		Mesh* mesh_res = doCast<Mesh>(unit);
		if (!mesh_res) {
			LOG(LOG_ERROR) << "GraphicsMeshManager::createElement: No mesh.";
			return nullptr;
		}
		bool dynamic_usage = mesh_res->isDynamic();
		GraphicsMesh* mesh_el = new GraphicsMesh();
		Poseidon::SceneManager* scene_mgr = core->get_scene_manager();

		Poseidon::Mesh* mesh = new Poseidon::Mesh(dynamic_usage);
		mesh->setViewport(scene_mgr
			->getViewport());
		mesh_el->m_mesh = mesh;

		// make sure we have a material
		Rhea::Material* material = mesh_res->getMaterial();
		if (!material) {
			LOG(LOG_ERROR) << unit->getPath() + ": item has no material";
			delete el;
			return nullptr;
		}
		// make sure material has graphics element
		if (!material->getElement("graphics")) {
			if (!m_service->addElement(material)) {
				LOG(LOG_ERROR) << "GraphicsItemManager::createElement: could not create element for material";
				delete el;
				return nullptr;
			}
		}

		// set the material in core
		setMaterial(core, material);

		unit->addElement("graphics", mesh_el);

		// initial initialization
		onUpdate(nullptr, unit);

		unit->registerEventHandler("update", this, std::bind(&GraphicsMeshManager::onUpdate, this, std::placeholders::_1, std::placeholders::_2));
		unit->registerEventHandler("delete", this, std::bind(&GraphicsMeshManager::onDelete, this, std::placeholders::_1, std::placeholders::_2));

		return mesh_el;
	}

	bool GraphicsMeshManager::onUpdate(Event* ev, Unit* unit)
	{
		GraphicsMesh* mesh_el = (GraphicsMesh*)unit->getElement("graphics");
		Mesh* mesh = doCast<Mesh>(unit);
		if (!mesh) {
			LOG(LOG_ERROR) << "GraphicsMeshManager::onUpdate: No mesh.";
			return nullptr;
		}

		// add vertex and index buffer

		std::vector<Mesh::vertex> vertices;
		std::vector<unsigned long> v_indices;

		mesh->getBuffers(vertices, v_indices);

		if (!vertices.size() || !v_indices.size()) {
			printf("Vertex is empty\n");
			return true;
		}

		std::vector<Hippios::vertex_uv> hippios_vertices;
		for (auto it = vertices.begin(); it != vertices.end(); ++it) {
			Mesh::vertex v = *it;
			hippios_vertices.push_back(Hippios::vertex_uv(v.x, v.y, v.z, v.u, v.v));
		}

		mesh_el->m_mesh->setVertices(sizeof(Hippios::vertex_uv), hippios_vertices.size(), hippios_vertices.data());
		mesh_el->m_mesh->setIndices(sizeof(unsigned long), v_indices.size() / 3, v_indices.data());
		return false;
	}

	bool GraphicsMeshManager::onDelete(Event* ev, Unit* unit)
	{
		unit->addElement("graphics", nullptr);
		return false;
	}
}