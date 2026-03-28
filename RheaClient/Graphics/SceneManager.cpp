#include <Poseidon/SceneManager.h>
#include <Poseidon/Material.h>
#include <Poseidon/Shader.h>
#include <Math/Matrix3x4.h>
#include <Math/Matrix4.h>

#include "SceneManager.h"
#include "ItemManager.h"
#include <Rhea/Graphics/CameraManager.h>
#include <Rhea/Unit.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Core/Item2D.h>
#include <Rhea/Graphics/GraphicsService.h>
#include <Psio/Log.h>
#include <Rhea/Core/Lighting.h>

namespace Rhea
{
	const std::vector<Item*>& SceneRenderState::getItems()
	{
		if (!m_items_valid) {
			m_items = m_scene->getItems(); // your existing Scene API
			m_items_valid = true;
		}
		return m_items;
	}	

	void GraphicsSceneManager::addItem(Unit* item)
	{
		//printf("GraphicsSceneManager::addItem: %s add typename: %s\n", item->getPathName().c_str(), item->getTypeName().c_str());

		// try manager
		GraphicsManager* mgr = m_service->getManager(item->getType());
		if (mgr) {
			mgr->Ensure(m_core, item);
		}
		else if (doCast<Item>(item)) {
			mgr = m_service->getManager<Item>();
			mgr->Ensure(m_core, item);
		}
		else if (doCast<Lighting>(item)) {
			Poseidon::SceneManager* scene_mgr = m_core->get_scene_manager();
			if (scene_mgr) {
				std::string render_context = "default";
				scene_mgr->getViewport()->getRenderContext(render_context)->setAmbient(doCast<Lighting>(item)->getAmbient());
			}		
		}
	}

	bool GraphicsSceneManager::onItemAdd(Event* ev, Unit* unit)
	{		
		Item* item = doCast<Item>(ev->getTarget());
		if (item)
			addItem(item);

		return false;
	}

	bool GraphicsSceneManager::Ensure(Poseidon::Core* core, Unit* unit)
	{
		Scene* scene = doCast<Scene>(unit);
		if (!scene) {
			LOG(LOG_ERROR) << "GraphicsSceneManager::Ensure: unit is not a scene ";
			return false;
		}

		// Already initialized? Use SceneRenderState as the marker.
		if (scene->getComponent<SceneRenderState>()) {
			return false;
		}

		// Create scene render state
		scene->addComponentUnique<SceneRenderState>(scene);

		// Initialize: add existing items + register handler
		m_core = core;

		scene->traverse([&](std::string& name, Component* c)->bool {
			Unit* u = doCast<Unit>(c);
			if (u && c != scene) addItem(u);
			return true;
			});

		scene->registerEventHandler(
			"add_item", this,
			std::bind(&GraphicsSceneManager::onItemAdd, this, std::placeholders::_1, std::placeholders::_2)
		);

		return true;
	}
}