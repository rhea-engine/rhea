#include <Poseidon/Entity.h>
#include <Poseidon/FrameRenderer.h>
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
#include <RheaClient/Graphics/Texture.h>
#include <RheaClient/Graphics/GraphicsService.h>

#include <Psio/Log.h>
#include <Rhea/Core/Light.h>
#include <RheaClient/Graphics/MaterialManager.h>
#include <RheaClient/Components/RenderComponent.h>
#include "MeshManager.h"
#include <Rhea/Components/MeshComponent.h>
#include <Poseidon/RenderWorld.h>
#include "TransformListener.h"

namespace rhea::client {
	GraphicsItemManager::~GraphicsItemManager()
	{
	
	}

	bool GraphicsItemManager::Ensure(poseidon::Core* core, rhea::Unit* unit)
	{
		rhea::Item* item = doCast<rhea::Item>(unit);
		if (!item) return false;

		rhea::Light* light = doCast<rhea::Light>(item);
		if (light) {
			return m_service->getManager<rhea::Light>()->Ensure(core, light);
		}

		// register event handlers for delete and update - these will be called even if the item is not renderable, so we can catch changes to renderable state and mesh assignment
		static auto deleteHandler = std::bind(&GraphicsItemManager::onItemDelete, this, std::placeholders::_1, std::placeholders::_2);
		static auto updateHandler = std::bind(&GraphicsItemManager::onUpdate, this, std::placeholders::_1, std::placeholders::_2);
		
		if (!unit->hasEventHandler("delete", this)) {
			unit->registerEventHandler("delete", this, deleteHandler);
		}
		if (!unit->hasEventHandler("update", this)) {
			unit->registerEventHandler("update", this, updateHandler);
		}

		// Early exit: nicht renderable - Handler sind trotzdem registriert
		if (!item->isRenderable()) {
			return true;
		}

		// Check if already exists
		auto* rc = item->getComponent<RenderComponent>();

		if (!rc) rc = item->addComponentUnique<RenderComponent>("renderer");

		if (rc->hasRenderable())
			return true;

		auto* tl = item->getComponent<TransformListener>();
		if (!tl) {
			tl = item->addComponentUnique<TransformListener>("transform_listener");
			tl->setGraphicsManager(this);
		}

		// Mesh handling
		rhea::MeshComponent* mc = item->getComponent<rhea::MeshComponent>();
		if (!mc) {
			// No MeshComponent - Handler registered, will be caught later
			return true;
		}

		rhea::Mesh* mesh = mc->getMesh();
		if (!mesh) {
			// Try to resolve the mesh
			rhea::Application* app = m_service->getApplication();
			if (!app) { 
				LOG(LOG_ERROR) << "Graphics service has no app"; 
				return false; 
			}

			mesh = doCast<rhea::Mesh>(app->getUnitTree()->find(
				mc->meshRef(), mc->basepathRef(), false));
			
			if (!mesh) {
				// Mesh not found yet - Handlers are registered, wait for update
				return true;
			}
			
			mc->setMesh(mesh);
			mesh->ready();
		}

		// create GPU mesh
		return createGPUResources(core, item, rc, mesh);
	}

	bool GraphicsItemManager::onUpdate(rhea::Event* ev, rhea::Unit* unit)
	{
		rhea::Item* item = doCast<rhea::Item>(unit);
		if (!item) return false;

		// Not renderable - clean up GPU resources if present
		if (!item->isRenderable()) {
			auto* rc = item->getComponent<RenderComponent>();
			if (rc && rc->hasRenderable()) {
				if (rc->ctx)
					rc->ctx->removeRenderChild(item);
				m_service->getEngine()->getRenderWorld()->destroyRenderable(rc->renderable);
				rc->renderable = poseidon::InvalidRenderable;				
				rc->gpuMesh = nullptr;
			}
			return true;
		}

		auto* rc = item->getComponent<RenderComponent>();
		if (!rc) {
			// Newly renderable - create RenderComponent
			rc = item->addComponentUnique<RenderComponent>("renderer");
		}
		auto* tl = item->getComponent<TransformListener>();
		if (!tl) {
			tl = item->addComponentUnique<TransformListener>("transform_listener");
			tl->setGraphicsManager(this);
		}

		rhea::MeshComponent* mc = item->getComponent<rhea::MeshComponent>();
		if (!mc) {
			// No mesh - nothing to do
			return false;
		}

		rhea::Mesh* mesh = mc->getMesh();
		if (!mesh) {
			// Try to resolve the mesh again
			rhea::Application* app = m_service->getApplication();
			if (app) {
				mesh = doCast<rhea::Mesh>(app->getUnitTree()->find(
					mc->meshRef(), mc->basepathRef(), false));
				if (mesh) {
					mc->setMesh(mesh);
					mesh->ready();
				}
			}
		}

		if (!mesh) {
			// Still no mesh
			return false;
		}

		// create/update GPU resources
		poseidon::Mesh* gpu = rc->gpuMesh;
		if (!gpu) {
			return createGPUResources(m_service->getEngine(), item, rc, mesh);
		}

		// Update: actualize Materials
		poseidon::Core* core = m_service->getEngine();
		auto* matMgr = static_cast<GraphicsMaterialManager*>(
			m_service->getManager<rhea::Material>());

		for (size_t i = 0; i < mesh->getNumSubmeshes(); i++) {
			rhea::Material* material = mesh->getMaterial(i);
			if (!material) {
				LOG(LOG_ERROR) << "GraphicsItemManager::onUpdate: No material for submesh " << i;
				continue;
			}

			poseidon::Material* gpuMat =
				m_service->materialCache().getOrCreate(
					core, material,
					[&](poseidon::Core* c, rhea::Material* m, 
					    poseidon::Material* outMat, bool fresh) -> bool {
						return matMgr->FillPoseidonMaterial(c, m, outMat, fresh);
					});
			
			if (gpuMat) {
				poseidon::Mesh::SubMesh* sub = gpu->getSubMesh(i);
				if (sub) sub->material = gpuMat;
			}
		}

		syncRenderable(core, item, rc);
		return false;
	}

	void GraphicsItemManager::onTransformUpdate(rhea::Unit* unit)
	{
		rhea::Item* item = doCast<rhea::Item>(unit);
		if (!item) return;
		auto* rc = item->getComponent<RenderComponent>();
		if (!rc || !rc->hasRenderable()) return;
		syncRenderable(m_service->getEngine(), item, rc);
	}

	bool GraphicsItemManager::onItemDelete(rhea::Event* ev, rhea::Unit* unit)
	{
		rhea::Item* item = doCast<rhea::Item>(unit);
		if (!item) return true;

		auto* rc = item->getComponent<RenderComponent>();
		if (!rc) return true;

		if (rc->hasRenderable()) {

			if (rc->ctx)
				rc->ctx->removeRenderChild(item);

			m_service->getEngine()->getRenderWorld()->destroyRenderable(rc->renderable);
			rc->renderable = poseidon::InvalidRenderable;
		}
		if (rc->hasLight()) {
			if (rc->ctx)
				rc->ctx->removeLight(rc->light);
			m_service->getEngine()->getRenderWorld()->destroyLight(rc->light);
			rc->light = poseidon::InvalidLight;
		}
		if (rc->hasCamera()) {
			if (rc->ctx) {
				rc->ctx->setCameraHandle(poseidon::InvalidCamera);
				rc->ctx->setCamera(nullptr);
			}
			m_service->getEngine()->getRenderWorld()->destroyCamera(rc->camera);
			rc->camera = poseidon::InvalidCamera;
		}

		rc->ctx = nullptr;
		rc->gpuMesh = nullptr;
		rc->skeleton = nullptr;

		return false;
	}

	// New helper method for GPU resource creation
	bool GraphicsItemManager::createGPUResources(poseidon::Core* core, 
                                                  rhea::Item* item,
                                                  RenderComponent* rc, 
                                                  rhea::Mesh* mesh)
	{
		if (!mesh) {
			LOG(LOG_ERROR) << "GraphicsItemManager::createGPUResources: mesh is null";
			return false;
		}

		// Ensure the mesh is managed by MeshManager
		// This will register event handlers and create GPU resources
		auto* meshMgr = static_cast<GraphicsMeshManager*>(
			m_service->getManager<rhea::Mesh>());
		
		if (!meshMgr->Ensure(core, mesh)) {
			LOG(LOG_ERROR) << "GraphicsItemManager::createGPUResources: MeshManager::Ensure failed for mesh '" 
			               << mesh->getPathName() << "'";
			return false;
		}

		// Get shared GPU mesh from cache (should exist after Ensure)
		poseidon::Mesh* shared = m_service->meshCache().find(mesh);
		
		if (!shared) {
			LOG(LOG_ERROR) << "  -> FAILED: GPU mesh not in cache after Ensure for '" 
			               << item->getPathName() << "'";
			return false;
		}
		
		rc->gpuMesh = shared;
		
		// Skeleton handling
		rhea::Skeleton* skeleton = mesh->getComponent<rhea::Skeleton>();
		if (skeleton) {
			rc->skeleton = skeleton;
		}

		// Create render entity if missing
		if (!rc->hasRenderable()) {
			rc->renderable = createRenderable(rc, core, item, shared, "default");
			
			if (rc->renderable == poseidon::InvalidRenderable) {
				LOG(LOG_ERROR) << "  -> FAILED: createRenderable returned invalid handle for '" 
				               << item->getPathName() << "'";
				return false;
			}

			if (!syncRenderable(core, item, rc)) {
				LOG(LOG_ERROR) << "GraphicsItemManager::createGPUResources: syncRenderable failed for '"
					<< item->getPathName() << "'";
				return false;
			}
		}

		return true;
	}

	bool GraphicsItemManager::syncRenderable(poseidon::Core* core, rhea::Item* item, RenderComponent* rc)
	{
		if (!core || !item || !rc || !rc->hasRenderable())
			return false;

		poseidon::Renderable* renderable = core->getRenderWorld()->resolveRenderable(rc->renderable);
		if (!renderable)
			return false;

		// mesh
		renderable->mesh = rc->gpuMesh;

		auto* vis = item->getComponent<rhea::VisualComponent>();
		if (vis) {
			renderable->visible = vis->isVisible();
		//	renderable->castsShadow = vis->castsShadow();
		//	renderable->receivesShadow = vis->receivesShadow();
		//	renderable->renderLayers = vis->getRenderLayers();
		}

		// optional migration/debug reference

		// skeleton
		renderable->skeleton = rc->skeleton;

		return syncTransform(core, item, rc);
	}

	bool GraphicsItemManager::syncTransform(poseidon::Core* core, rhea::Item* item, RenderComponent* rc)
	{
		poseidon::Renderable* renderable = core->getRenderWorld()->resolveRenderable(rc->renderable);
		if (!renderable)
			return false;

		// transform copy
		rhea::Transform* transform = item->getTransform();
		if (transform) {
			Position* position = transform->get_derived_position();
			if (position) {
				math::Matrix3x4 model_wtm = position->getWorldTransform();

				math::Matrix4 world = model_wtm;
				math::Matrix4 inverse = world.inverse().transpose();

				renderable->worldTransform = world;
				renderable->inverseTranspose = inverse;
			}
		}
		return true;
	}
}
