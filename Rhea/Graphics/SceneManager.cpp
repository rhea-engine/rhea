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

namespace Rhea
{
	void GraphicsSceneManager::createScene(Poseidon::Core* core, GraphicsScene *elem, Scene* scene)
	{
		Poseidon::SceneManager* scene_mgr = core->get_scene_manager();
		m_core = core;

		// add my items
		scene->traverse([&](std::string& name, Unit* unit)->bool {
			{
				if (unit != scene) {
					addItem(unit);
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
	}

	bool GraphicsSceneManager::onItemAdd(Event* ev, Unit* unit)
	{		
		Item* item = static_cast<Item*>(ev->getTarget());
		addItem(item);

		return false;
	}

	Element* GraphicsSceneManager::createElement(Poseidon::Core* core, Unit* unit)
	{
		// check if already created
		Element* el = unit->getElement("graphics");
		if (el)
			return el;

		// create it
		GraphicsScene* scene_elem = new GraphicsScene(static_cast<Scene*>(unit));
		createScene(core, scene_elem, static_cast<Scene*>(unit));

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