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

namespace rhea::client {
	const std::vector<rhea::Item* >& SceneRenderState::getItems()
	{
		if (!m_items_valid) {
			m_items = m_scene->getItems(); // your existing Scene API
			m_items_valid = true;
		}
		return m_items;
	}	

	void GraphicsSceneManager::addItem(rhea::Unit* item)
	{
		//printf("GraphicsSceneManager::addItem: %s add typename: %s\n", item->getPathName().c_str(), item->getTypeName().c_str());

		// try manager
		GraphicsManager* mgr = m_service->getManager(item->getType());
		if (mgr) {
			mgr->Ensure(m_core, item);
		}
		else if (doCast<rhea::Item>(item)) {
			mgr = m_service->getManager<rhea::Item>();
			mgr->Ensure(m_core, item);
		}
		else if (doCast<rhea::Lighting>(item)) {
			poseidon::SceneManager* scene_mgr = m_core->get_scene_manager();
			if (scene_mgr) {
				std::string render_context = "default";
				scene_mgr->getViewport()->getRenderContext(render_context)->setAmbient(doCast<rhea::Lighting>(item)->getAmbient());
			}		
		}
	}

	bool GraphicsSceneManager::onItemAdd(rhea::Event* ev, rhea::Unit* unit)
	{		
		rhea::Item* item = doCast<rhea::Item>(ev->getTarget());
		if (item)
			addItem(item);

		return false;
	}

	bool GraphicsSceneManager::Ensure(poseidon::Core* core, rhea::Unit* unit)
	{
		rhea::Scene* scene = doCast<rhea::Scene>(unit);
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

		scene->traverse([&](std::string& name, rhea::Component* c)->bool {
			rhea::Unit* u = doCast<rhea::Unit>(c);
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