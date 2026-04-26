#include "Rhea/Core/Scene.h"

#include <memory>

#include "Rhea/Core/Item.h"
#include "Rhea/Core/SceneLoader.h"
#include "Rhea/Event.h"
#include "Rhea/Application.h"
#include "Rhea/UnitTree.h"
#include <Psio/Log.h>

namespace rhea
{
	void Scene::onParented()
	{		
		LOG(LOG_NOTICE) << "scene ready";
		ready(); 
	}

	void Scene::addItem(Item* item)
	{
		m_items.push_back(item);
		Event e("add_item", item);
		sendEvent("add_item", &e);
	}	

	void Scene::removeItem(Item* item)
	{
		m_items.erase(std::remove(m_items.begin(), m_items.end(), item), m_items.end());
		Event e("remove_item", item);
		sendEvent("remove_item", &e);
	}

	void Scene::ready()
	{
		Unit::ready();

		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Item* child = doCast<Item>(it->second);
			if (child)
				m_items.push_back(child);
		}
		registerEventHandler("scene_enter", this, std::bind(&Scene::onSceneEnter, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("attached_item", this, std::bind(&Scene::onItemAttached, this, std::placeholders::_1, std::placeholders::_2));
		registerEventHandler("detached_item", this, std::bind(&Scene::onItemDetached, this, std::placeholders::_1, std::placeholders::_2));
	}

	bool Scene::onSceneEnter(Event* ev, Unit* unit)
	{
		// we capture this event
		ev->stopPropagation();
		// send add item
		Event e("add_item", ev->getTarget());
		sendEvent("add_item", &e);
		return true;
	}

	bool Scene::onItemAttached(Event* ev, Unit* unit)
	{
		// we capture this event
		ev->stopPropagation();

		if (m_is_active) {
			// send add item for subtree
			Item* item = doCast<Item>(ev->getTarget());
			Event e("add_item", item);
			sendEvent("add_item", &e);

			item->children([&](std::string evname, Component* child)->bool {
				Item* child_item = doCast<Item>(child);
				if (child_item) {
					Event e("add_item", child_item);
					sendEvent("add_item", &e);
				}
				return true;
				});
		}
		return true;
	}

	bool Scene::onItemDetached(Event* ev, Unit* unit)
	{
		// we capture this event
		ev->stopPropagation();

		if (m_is_active) {
			// send add item for subtree
			Item* item = doCast<Item>(ev->getTarget());
			Event e("remove_item", item);
			sendEvent("remove_item", &e);

			item->children([&](std::string evname, Component* child)->bool {
				Item* child_item = doCast<Item>(child);
				if (child_item) {
					Event e("remove_item", child_item);
					sendEvent("remove_item", &e);
				}
				return true;
				});
		}
		return true;
	}

	void Scene::show()
	{
		if (!m_app) {
			LOG(LOG_ERROR) << "Scene: No app pointer set, did you forget to call setup?";
			return;
		}

		// the scene needs to be active
		if (!m_is_active)
			activate();

		rhea::Event ev("scene_show", this);
		sendEventChildren(std::string("scene_show"), &ev);
		broadcast("show");
		m_app->onScene(this);
	}

	void Scene::hide()
	{
		rhea::Event ev("scene_hide", this);
		sendEventChildren("scene_hide", &ev);
	}

	void Scene::activate()
	{
		Event e("scene_activate", this);
		m_app->getUnitTree()->sendEvent("scene_activate", &e);
		m_is_active = true;
	}

	void Scene::deactive()
	{
		Event e("scene_deactivate", this);
		m_app->getUnitTree()->sendEvent("scene_deactivate", &e);
		m_is_active = false;
	}
}