#include "MeshManager.h"
#include <Rhea/Core/Item.h>
#include <Rhea/Core/Mesh.h>
#include <Poseidon/Mesh.h>
#include <Poseidon/FrameRenderer.h>
#include <Psio/Log.h>
#include <Rhea/Core/Material.h>
#include <RheaClient/Graphics/GraphicsService.h>
#include <Rhea/Resources/VertexBuffer.h>
#include <RheaClient/Graphics/MaterialManager.h>

namespace rhea::client {
	bool GraphicsMeshManager::Ensure(poseidon::Core* core, rhea::Unit* unit)
	{
		rhea::Mesh* r_mesh = doCast<rhea::Mesh>(unit);
		if (!r_mesh) return false;

		// Check if this mesh has already failed
		std::string meshKey = r_mesh->getPath();
		if (m_failed_meshes.count(meshKey)) {
			return false;
		}

		poseidon::Mesh* gpu = m_service->meshCache().getOrCreate(
			core, r_mesh, r_mesh->isDynamic(),
			[&](poseidon::Core* c, rhea::Mesh* m, poseidon::Mesh* out) {
				return FillPoseidonMesh(c, m, out);
			});

		if (!gpu) {
			// Build failed -> add to negative cache
			m_failed_meshes.insert(meshKey);
			LOG(LOG_ERROR) << "GraphicsMeshManager::Ensure: GPU mesh creation failed for '" << meshKey << "', will not retry automatically.";
			return false;
		}

		if (!unit->hasEventHandler("update", this))
			unit->registerEventHandler("update", this, std::bind(&GraphicsMeshManager::onUpdate, this, std::placeholders::_1, std::placeholders::_2));
		//unit->registerEventHandler("delete", this, std::bind(&GraphicsMeshManager::onDelete, this, std::placeholders::_1, std::placeholders::_2));

		return true;
	}

	bool GraphicsMeshManager::ensureMaterialElement(rhea::Material* material)
	{
		if (!material)
			return false;

		poseidon::Core* core = m_service->getEngine();
		if (!core)
			return false;

		// Ask cache to create or reuse GPU material
		auto* meshMgr = static_cast<GraphicsMaterialManager*>(m_service->getManager<rhea::Material>());
		poseidon::Material* gpuMat =
			m_service->materialCache().getOrCreate(
				core,
				material,
				[&](poseidon::Core* c, rhea::Material* mat_unit, poseidon::Material* out, bool fresh) -> bool
				{
					return meshMgr->FillPoseidonMaterial(c, mat_unit, out, fresh);
				});

		return gpuMat != nullptr;		
	}

	bool GraphicsMeshManager::onUpdate(rhea::Event* ev, rhea::Unit* unit)
	{
		rhea::Mesh* meshUnit = doCast<rhea::Mesh>(unit);
		if (!meshUnit) return true;

		std::string meshKey = meshUnit->getPath();

		// If already marked as failed, do not retry
		if (m_failed_meshes.count(meshKey)) {
			return false;
		}

		// Try to get cached GPU mesh
		poseidon::Mesh* gpu = m_service->meshCache().find(meshUnit);
		if (!gpu) {
			gpu = m_service->meshCache().getOrCreate(
				m_service->getEngine(),
				meshUnit,
				meshUnit->isDynamic(),
				[&](poseidon::Core* c, rhea::Mesh* m, poseidon::Mesh* out) {
					return FillPoseidonMesh(c, m, out);
				});
			if (!gpu) {
				m_failed_meshes.insert(meshKey);
				LOG(LOG_ERROR) << "GraphicsMeshManager::onUpdate: GPU mesh creation failed for '" << meshKey << "', will not retry.";
				return false;
			}
		}
		
		// Refill cached poseidon mesh buffers
		if (!FillPoseidonMesh(m_service->getEngine(), meshUnit, gpu)) {
			LOG(LOG_ERROR) << "GraphicsMeshManager::onUpdate: FillPoseidonMesh failed for '" << meshKey << "'";
			return false;
		}
		
		return false;		
	}

	bool GraphicsMeshManager::FillPoseidonMesh(poseidon::Core* core, rhea::Mesh* mesh, poseidon::Mesh* out)
	{
		if (!mesh || !out) return false;

		// Add vertex and index buffer
		rhea::VertexBuffer* vbuf = mesh->getVertexBuffer();
		if (vbuf && vbuf->size()) {
			out->setVertices(vbuf->getVertexSize(), vbuf->size(), vbuf->data());
			for (size_t idx = 0; idx < mesh->getNumSubmeshes(); idx++) {
				std::vector<uint32_t> v_indices = mesh->getIndices(idx);
				// Check if v_indices is valid and has a multiple of 3 elements
				if (!v_indices.size() || (v_indices.size() % 3 != 0)) {
					LOG(LOG_ERROR) << "Invalid index buffer";
					return false;
				}

				rhea::Material* mat = mesh->getMaterial(idx);
				if (!mat) {
					LOG(LOG_ERROR) << "GraphicsMeshManager::FillPoseidonMesh: No material for submesh " << idx;
					return false;
				}
				poseidon::Material* gpuMat =
					m_service->materialCache().getOrCreate(
						core,
						mat,
						[&](poseidon::Core* c, rhea::Material* m, poseidon::Material* outMat, bool fresh) -> bool {							
							auto* meshMgr = static_cast<GraphicsMaterialManager*>(m_service->getManager<rhea::Material>());
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
			std::vector<rhea::Mesh::vertex> vertices;
			std::vector<uint32_t> v_indices;

			mesh->getBuffers(vertices, v_indices);

			if (!vertices.size() || !v_indices.size()) {				
				// Empty mesh is valid (e.g., invisible or empty text widget)
				// Just don't add any geometry, but don't treat it as an error
				return true;
			}

			std::vector<hippios::vertex_uv> hippios_vertices;
			for (auto& v: vertices) {				
				hippios_vertices.push_back(hippios::vertex_uv(v.x, v.y, v.z, v.u, v.v));
			}

			out->setVertices(sizeof(hippios::vertex_uv), hippios_vertices.size(), hippios_vertices.data());
			
			rhea::Material* mat0 = mesh->getMaterial(0);
			if (!mat0) {
				LOG(LOG_ERROR) << "GraphicsMeshManager::FillPoseidonMesh: No material(0) ";
				return false;
			}
			poseidon::Material* gpuMat0 =
				m_service->materialCache().getOrCreate(
					core,
					mat0,
					[&](poseidon::Core* c, rhea::Material* m, poseidon::Material* outMat, bool fresh) -> bool {
						auto* materialMgr = static_cast<GraphicsMaterialManager*>(m_service->getManager<rhea::Material>());
						return materialMgr->FillPoseidonMaterial(c, m, outMat, fresh);
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

	bool GraphicsMeshManager::onDelete(rhea::Event* ev, rhea::Unit* unit)
	{
		// On deletion, also remove from negative cache
		if (rhea::Mesh* meshUnit = doCast<rhea::Mesh>(unit)) {
			m_failed_meshes.erase(meshUnit->getPath());
		}
		return false;
	}
}