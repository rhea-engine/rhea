#include "MeshManager.h"
#include <Rhea/Core/Item.h>
#include <Rhea/Core/Mesh.h>
#include <Poseidon/Mesh.h>
#include <Poseidon/SceneManager.h>
#include <Psio/Log.h>
#include <Rhea/Core/Material.h>
#include <Rhea/Graphics/GraphicsService.h>
#include <Rhea/Resources/VertexBuffer.h>
#include <Rhea/Graphics/MaterialManager.h>

namespace Rhea
{
	bool GraphicsMeshManager::Ensure(Poseidon::Core* core, Unit* unit)
	{
		Mesh* r_mesh = doCast<Mesh>(unit);
		if (!r_mesh) return false;

		Poseidon::Mesh* gpu = m_service->meshCache().getOrCreate(
			core, r_mesh, r_mesh->isDynamic(),
			[&](Poseidon::Core* c, Mesh* m, Poseidon::Mesh* out) {
				return FillPoseidonMesh(c, m, out);
			});

		unit->registerEventHandler("update", this, std::bind(&GraphicsMeshManager::onUpdate, this, std::placeholders::_1, std::placeholders::_2));
		//unit->registerEventHandler("delete", this, std::bind(&GraphicsMeshManager::onDelete, this, std::placeholders::_1, std::placeholders::_2));

		return true;
	}

	bool GraphicsMeshManager::ensureMaterialElement(Material* material)
	{
		if (!material)
			return false;

		Poseidon::Core* core = m_service->getEngine();
		if (!core)
			return false;

		// Ask cache to create or reuse GPU material
		auto* meshMgr = static_cast<GraphicsMaterialManager*>(m_service->getManager<Material>());
		Poseidon::Material* gpuMat =
			m_service->materialCache().getOrCreate(
				core,
				material,
				[&](Poseidon::Core* c, Material* mat_unit, Poseidon::Material* out, bool fresh) -> bool
				{
					return meshMgr->FillPoseidonMaterial(c, mat_unit, out, fresh);
				});

		return gpuMat != nullptr;		
	}

	bool GraphicsMeshManager::onUpdate(Event* ev, Unit* unit)
	{
		Mesh* meshUnit = doCast<Mesh>(unit);
		if (!meshUnit) return true;

		// Try to get cached GPU mesh
		Poseidon::Mesh* gpu = m_service->meshCache().find(meshUnit);
		if (!gpu) {
			gpu = m_service->meshCache().getOrCreate(
				m_service->getEngine(),
				meshUnit,
				meshUnit->isDynamic(),
				[&](Poseidon::Core* c, Mesh* m, Poseidon::Mesh* out) {
					return FillPoseidonMesh(c, m, out);
				});
			if (!gpu) return true;
		}
		
		// Refill cached poseidon mesh buffers
		FillPoseidonMesh(m_service->getEngine(), meshUnit, gpu);
		
		return false;		
	}

	bool GraphicsMeshManager::FillPoseidonMesh(Poseidon::Core* core, Mesh* mesh, Poseidon::Mesh* out)
	{
		if (!mesh || !out) return false;

		// add vertex and index buffer
		VertexBuffer* vbuf = mesh->getVertexBuffer();
		if (vbuf && vbuf->size()) {
			out->setVertices(vbuf->getVertexSize(), vbuf->size(), vbuf->data());
			for (size_t idx = 0; idx < mesh->getNumSubmeshes(); idx++) {
				std::vector<unsigned long> v_indices = mesh->getIndices(idx);
				// Check if v_indices is valid and has a multiple of 3 elements
				if (!v_indices.size() || (v_indices.size() % 3 != 0)) {
					LOG(LOG_ERROR) << "Invalid index buffer";
					return false;
				}

				Material* mat = mesh->getMaterial(idx);
				if (!mat) {
					LOG(LOG_ERROR) << "GraphicsMeshManager::FillPoseidonMesh: No material for submesh " << idx;
					return false;
				}
				Poseidon::Material* gpuMat =
					m_service->materialCache().getOrCreate(
						core,
						mat,
						[&](Poseidon::Core* c, Material* m, Poseidon::Material* outMat, bool fresh) -> bool {							
							auto* meshMgr = static_cast<GraphicsMaterialManager*>(m_service->getManager<Material>());
							return meshMgr->FillPoseidonMaterial(c, m, outMat, fresh);
						}
					);
				if (!gpuMat) {
					LOG(LOG_ERROR) << "GraphicsMeshManager::FillPoseidonMesh: Failed to build GPU material for " << mat->getPathName();
					return false;
				}				
				out->setMesh(idx, sizeof(unsigned long), v_indices.size() / 3, v_indices.data(), gpuMat);
			}
		}
		else {
			// Fallback path: CPU vertices + indices
			std::vector<Mesh::vertex> vertices;
			std::vector<unsigned long> v_indices;

			mesh->getBuffers(vertices, v_indices);

			if (!vertices.size() || !v_indices.size()) {
				LOG(LOG_ERROR) << "Vertex is empty";
				return true;
			}

			std::vector<Hippios::vertex_uv> hippios_vertices;
			for (auto& v: vertices) {				
				hippios_vertices.push_back(Hippios::vertex_uv(v.x, v.y, v.z, v.u, v.v));
			}

			out->setVertices(sizeof(Hippios::vertex_uv), hippios_vertices.size(), hippios_vertices.data());
			
			Material* mat0 = mesh->getMaterial(0);
			if (!mat0) {
				LOG(LOG_ERROR) << "GraphicsMeshManager::FillPoseidonMesh: No material(0) ";
				return false;
			}
			Poseidon::Material* gpuMat0 =
				m_service->materialCache().getOrCreate(
					core,
					mat0,
					[&](Poseidon::Core* c, Material* m, Poseidon::Material* outMat, bool fresh) -> bool {
						auto* meshMgr = static_cast<GraphicsMaterialManager*>(m_service->getManager<Material>());
						return meshMgr->FillPoseidonMaterial(c, m, outMat, fresh);
					}
				);
			if (!gpuMat0) {
				LOG(LOG_ERROR) << "GraphicsMeshManager::FillPoseidonMesh: Failed to build GPU material for " << mat0->getPathName();
				return false;
			}
			out->setMesh(0, sizeof(unsigned long), v_indices.size() / 3, v_indices.data(), gpuMat0);
		}
		return true;
	}	

	bool GraphicsMeshManager::onDelete(Event* ev, Unit* unit)
	{
		return false;
	}
}