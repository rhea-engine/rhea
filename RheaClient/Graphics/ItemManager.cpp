#include <Poseidon/Entity.h>
#include <Poseidon/SceneManager.h>
#include <Poseidon/Material.h>
#include <Poseidon/Shader.h>
#include <Poseidon/RenderContext.h>

#include <Rhea/Application.h>
#include <Rhea/Unit.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Core/Mesh.h>
#include <Rhea/Core/Material.h>
#include <Rhea/Resources/TextResource.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Animation/Skeleton.h>

#include "ItemManager.h"
#include <Rhea/Graphics/Texture.h>
#include <Rhea/Graphics/GraphicsService.h>

#include <Psio/Log.h>
#include <Rhea/Core/Light.h>
#include <Rhea/Graphics/MaterialManager.h>
#include <Rhea/Components/RenderComponent.h>
#include "MeshManager.h"
#include <Rhea/Components/MeshComponent.h>

namespace rhea::client {
	GraphicsItemManager::~GraphicsItemManager()
	{
	
	}

	bool GraphicsItemManager::Ensure(Poseidon::Core* core, Rhea::Unit* unit)
	{
		Rhea::Item* item = doCast<Rhea::Item>(unit);
		if (!item) return false;

		// ignore if not renderable - this allows non-renderable items to still be in the scene and have their render components updated without creating GPU resources
		if (!item->isRenderable())
			return true;

		// check if already exists
		auto* rc = item->getComponent<RenderComponent>();
		if (rc && rc->entity)
			return true;

//		DLOG(LOG_DEBUG) << "GraphicsItemManager::Ensure called for: " << item->getPathName();

		if (!rc) rc = item->addComponentUnique<RenderComponent>("renderer");
		
		// special handling for light
		Rhea::Light* light = doCast<Rhea::Light>(item);
		if (light) {
			DLOG(LOG_DEBUG) << "GraphicsItemManager::Ensure: light";			
			Poseidon::Renderable* light_entity = static_cast<Poseidon::Renderable*>(createEntity(rc, core, unit, "default", false));			
		}
		else {
			Rhea::MeshComponent* mc = item->getComponent<Rhea::MeshComponent>();
			Rhea::Mesh* mesh = nullptr;
			if (!mc) {
				LOG(LOG_DEBUG) << "  -> Item '" << item->getPathName() << "' has no MeshComponent, skipping";
				return false;
			}
			if (!mc->getMesh()) {
				// Resolve mesh unit if needed
				
				Rhea::Application* app = m_service->getApplication();
				if (!app) { LOG(LOG_ERROR) << "Graphics service has no app"; return false; }

				mesh = doCast<Rhea::Mesh>(app->getUnitTree()->find(mc->meshRef(), mc->basepathRef(), false));
				if (!mesh) {
					LOG(LOG_ERROR) << "  -> FAILED: No mesh found for '" << mc->meshRef() << "'";
					return false;
				}
				else {
					mc->setMesh(mesh);
				}
				mesh->ready(); // ok for now
			}
			else
				mesh = mc->getMesh();

			auto* meshMgr = static_cast<GraphicsMeshManager*>(m_service->getManager<Rhea::Mesh>());

			// Get shared GPU mesh from cache 
			Poseidon::Mesh* shared = m_service->meshCache().getOrCreate(core, mesh, mesh->isDynamic(),
				[&](Poseidon::Core* core, Rhea::Mesh* mesh, Poseidon::Mesh* out)->bool {
					return meshMgr->FillPoseidonMesh(core, mesh, out);
				});
			if (!shared) {
				LOG(LOG_ERROR) << "  -> FAILED: GPU mesh creation failed for '" << item->getPathName() << "'";
				return false;
			}
			rc->gpuMesh = shared;
			Rhea::Skeleton* skeleton = mesh->getComponent<Rhea::Skeleton>();
			if (skeleton) {
				/*Poseidon::Skeleton* gpuSkeleton = meshMgr->getOrCreateGPUSkeleton(core, skeleton);
				if (!gpuSkeleton) {
					LOG(LOG_ERROR) << "  -> FAILED: GPU skeleton creation failed for '" << item->getPathName() << "'";
					return false;
				}
				*/
				rc->skeleton = skeleton;
			}
		
			// Create instance render entity using shared mesh
			rc->entity = static_cast<Poseidon::Renderable*>(
				createRenderable(rc, core, unit, shared, "default")
				);

			if (!rc->entity) {
				LOG(LOG_ERROR) << "  -> FAILED: createRenderable returned null for '" << item->getPathName() << "'";
				return false;
			}
		}
		
		// register handlers
		unit->registerEventHandler("delete", this, std::bind(&GraphicsItemManager::onItemDelete, this, std::placeholders::_1, std::placeholders::_2));
		unit->registerEventHandler("update", this, std::bind(&GraphicsItemManager::onUpdate, this, std::placeholders::_1, std::placeholders::_2));

		return true;
	}

	bool GraphicsItemManager::onUpdate(Rhea::Event* ev, Rhea::Unit* unit)
	{
		Rhea::Item* item = doCast<Rhea::Item>(unit);
		if (!item) return false;

		// ignore if not renderable - this allows non-renderable items to still be in the scene and have their render components updated without creating GPU resources
		if (!item->isRenderable())
			return true;

		// possible material change - check if material has element		
		auto* rc = item->getComponent<RenderComponent>();
		if (!rc) return false;
		Rhea::MeshComponent* mc = item->getComponent<Rhea::MeshComponent>();
		if (!mc || !mc->getMesh()) return false;
		Rhea::Mesh* mesh = mc->getMesh();

		// NOTE: This assumes a changed material will actually be a new material (i.e. the old one gets destroyed)

		// 1) Ensure GPU mesh exists (from cache)
		Poseidon::Mesh* gpu = rc->gpuMesh;
		if (!gpu) {
			auto* meshMgr = static_cast<GraphicsMeshManager*>(m_service->getManager<Rhea::Mesh>());

			gpu = m_service->meshCache().getOrCreate(
				m_service->getEngine(), // Poseidon::Core*
				mesh,
				mesh->isDynamic(),
				[&](Poseidon::Core* c, Rhea::Mesh* m, Poseidon::Mesh* out) {
					return meshMgr->FillPoseidonMesh(c, m, out);
				});

			if (!gpu) return false;
			rc->gpuMesh = gpu;
		}

		// lazy-create entity if missing
		if (!rc->entity && gpu) {
			rc->entity = static_cast<Poseidon::Renderable*>(
				createRenderable(rc, m_service->getEngine(), unit, gpu, "default")
				);
		}

		// 2) Ensure all submesh materials have GPU materials + apply them to gpu mesh
		Poseidon::Core* core = m_service->getEngine();
		auto* matMgr = static_cast<GraphicsMaterialManager*>(m_service->getManager<Rhea::Material>());

		for (size_t i = 0; i < mesh->getNumSubmeshes(); i++) {
			Rhea::Material* material = mesh->getMaterial(i);
			if (!material) {
				LOG(LOG_ERROR) << "GraphicsItemManager::onUpdate: No material for submesh " << i;
				continue;
			}

			// Ask cache for GPU material (create/rebuild if needed)
			Poseidon::Material* gpuMat =
				m_service->materialCache().getOrCreate(
					core,
					material,
					[&](Poseidon::Core* c, Rhea::Material* m, Poseidon::Material* outMat, bool fresh) -> bool {
						return matMgr->FillPoseidonMaterial(c, m, outMat, fresh);
					});
			if (!gpuMat) {
				LOG(LOG_ERROR) << "GraphicsItemManager::onUpdate: Failed to get GPU material for " << material->getPathName();
				continue;
			}
			
			// Apply Poseidon material to cached gpu mesh
			Poseidon::Mesh::SubMesh* sub = gpu->getSubMesh(i);
			if (!sub) continue;

			sub->material = gpuMat;			
		}

		return false;
	}

	bool GraphicsItemManager::onItemDelete(Rhea::Event* ev, Rhea::Unit* unit)
	{
		Rhea::Item* item = doCast<Rhea::Item>(unit);
		if (!item) return true;

		auto* rc = item->getComponent<RenderComponent>();
		if (rc) {

			if (rc->ctx)
				rc->ctx->removeRenderChild(item);

			if (rc->entity)
				delete rc->entity;

			rc->entity = nullptr;			
		}
		return false;
	}
}
