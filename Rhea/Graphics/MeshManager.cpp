#include "MeshManager.h"
#include <Rhea/Core/Item.h>
#include <Rhea/Fabrics/3D/Mesh.h>
#include <Poseidon/Mesh.h>
#include <Poseidon/SceneManager.h>
#include <Psio/Log.h>
#include <Rhea/Core/Material.h>
#include "GraphicsService.h"
#include <Rhea/Resources/VertexBuffer.h>
#include "MaterialManager.h"
#include "GraphicsComponent.h"

namespace Rhea
{
	GraphicsElement* GraphicsMeshManager::createElement(Poseidon::Core* core, Unit* unit)
	{
		// check if already created
		GraphicsComponent* el = unit->getComponent<GraphicsComponent>();
		if (el)
			return el->getElement();
		
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

		// make sure the materials of all submeshes have a graphics element
		for (size_t i = 0; i < mesh_res->getNumSubmeshes(); i++) {
			Material* material = mesh_res->getMaterial(i);
			if (!material) {
				LOG(LOG_ERROR) << "GraphicsMeshManager::createElement: No material for submesh " << i;
				delete el;
				return nullptr;
			}
			if (!material->getComponent<GraphicsComponent>()) {
				if (!m_service->addElement(material)) {
					LOG(LOG_ERROR) << "GraphicsItemManager::createElement: could not create element for material";
					delete el;
					return nullptr;
				}
			}
			// set the material in core
			setMaterial(core, material);
		}		

		unit->addComponentUnique<GraphicsComponent>(mesh_el);		

		// initial initialization
		onUpdate(nullptr, unit);

		unit->registerEventHandler("update", this, std::bind(&GraphicsMeshManager::onUpdate, this, std::placeholders::_1, std::placeholders::_2));
		//unit->registerEventHandler("delete", this, std::bind(&GraphicsMeshManager::onDelete, this, std::placeholders::_1, std::placeholders::_2));

		return mesh_el;
	}

	bool GraphicsMeshManager::ensureMaterialElement(Material* material)
	{
		if (material) {
			if (!material->getComponent<GraphicsComponent>()) {
				if (!m_service->addElement(material)) {
					LOG(LOG_ERROR) << "GraphicsMeshManager::ensureMaterialElement: could not create element for material";
					return false;
				}
				setMaterial(m_service->getEngine(), material);
			}
			return true;
		}
		else
			return false;
	}

	bool GraphicsMeshManager::onUpdate(Event* ev, Unit* unit)
	{
		GraphicsComponent* g_component = unit->getComponent<GraphicsComponent>();
		if (!g_component)
			return false;
		GraphicsMesh* mesh_el = (GraphicsMesh*)g_component->getElement();
		Mesh* mesh = doCast<Mesh>(unit);
		if (!mesh) {
			LOG(LOG_ERROR) << "GraphicsMeshManager::onUpdate: No mesh.";
			return false;
		}

		// add vertex and index buffer
		VertexBuffer* vbuf = mesh->getVertexBuffer();
		if (vbuf && vbuf->size()) {
			mesh_el->m_mesh->setVertices(vbuf->getVertexSize(), vbuf->size(), vbuf->data());
			for (size_t idx = 0; idx < mesh->getNumSubmeshes(); idx++) {
				std::vector<unsigned long> v_indices = mesh->getIndices(idx);
				// Check if v_indices is valid and has a multiple of 3 elements
				if (v_indices.size() && v_indices.size() % 3 == 0) {
					GraphicsComponent* mat_component = mesh->getMaterial(idx)->getComponent<GraphicsComponent>();
					if (mat_component)
					  mesh_el->m_mesh->setMesh(idx, sizeof(unsigned long), v_indices.size() / 3, v_indices.data(), ((GraphicsMaterial*)(mat_component->getElement()))->m_material);
				}
				else {
					LOG(LOG_ERROR) << "Invalid index buffer";
					return true;
				}
			}
		}
		else {
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
			ensureMaterialElement(mesh->getMaterial());
			GraphicsComponent* mat_component = mesh->getMaterial(0)->getComponent<GraphicsComponent>();
			if (mat_component)
				mesh_el->m_mesh->setMesh(0, sizeof(unsigned long), v_indices.size() / 3, v_indices.data(), ((GraphicsMaterial*)(mat_component->getElement()))->m_material);
		}
		return false;
	}

	bool GraphicsMeshManager::onDelete(Event* ev, Unit* unit)
	{
		return false;
	}
}