#include <Poseidon/SceneManager.h>
#include <Poseidon/Material.h>
#include <Poseidon/Shader.h>
#include <Math/Matrix3x4.h>
#include <Math/Matrix4.h>

#include "SceneManager.h"
#include "ItemManager.h"
#include "CameraManager.h"
#include "../Unit.h"
#include "Fabrics/3D/Item.h"
#include "Fabrics/Canvas/Item2D.h"
#include "GraphicsService.h"
#include <Psio/Log.h>
#include <Fabrics/Core/Lighting.h>

namespace Rhea
{
	void GraphicsSceneManager::createScene(Poseidon::Core* core, GraphicsScene *elem, Scene* scene)
	{
		Poseidon::SceneManager* scene_mgr = core->get_scene_manager();
		m_core = core;

		if (!scene) {
			LOG(LOG_ERROR) << "GraphicsSceneManager::createScene: no scene.";
			return;
		}

		// add my items
		scene->traverse([&](std::string& name, Component* unit)->bool {
			{
				Unit* u = doCast<Unit>(unit);
				if (u && unit != scene) {
					addItem(u);
				}
				return true;
			}
			});
		scene->addElement("graphics", elem);

		// listen to add item event		
		scene->registerEventHandler("add_item", this, std::bind(&GraphicsSceneManager::onItemAdd, this, std::placeholders::_1, std::placeholders::_2));
	}

	void GraphicsSceneManager::addItem(Unit* item)
	{
		printf("GraphicsSceneManager::addItem: %s add typename: %s\n", item->getPathName().c_str(), item->getTypeName().c_str());

		// try manager
		GraphicsManager* mgr = m_service->getManager(item->getTypeName());
		if (mgr) {
			mgr->createElement(m_core, item);
		}
		else if (doCast<Item>(item)) {
			mgr = m_service->getManager("//meworla/item");
			mgr->createElement(m_core, item);
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

	Element* GraphicsSceneManager::createElement(Poseidon::Core* core, Unit* unit)
	{
		Scene* scene = doCast<Scene>(unit);
		if (!scene) {
			LOG(LOG_ERROR) << "GraphicsSceneManager::createElement: no scene";
			return nullptr;
		}

		// check if already created
		Element* el = scene->getElement("graphics");
		if (el)
			return el;

		// create it
		GraphicsScene* scene_elem = new GraphicsScene(scene);
		createScene(core, scene_elem, scene);

		return scene_elem;
	}
	
	std::vector<Item*> GraphicsScene::getItems()
	{
		if (!m_items_valid) {
			// get items from scene tree
			m_items = m_scene->getItems();

			m_items_valid = true;
		}
		return m_items;
	}
}