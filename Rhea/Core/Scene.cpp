#include "Rhea/Core/Scene.h"

#include <memory>

#include "Rhea/Core/Item.h"
#include "Rhea/Core/SceneLoader.h"
#include "Rhea/Event.h"
#include "Rhea/Application.h"
#include "Rhea/UnitTree.h"
#include <Psio/Log.h>

namespace Rhea
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

	void Scene::show()
	{
		// the scene needs to be active
		if (!m_is_active)
			activate();

		Rhea::Event ev("scene_show", this);
		sendEventChildren(std::string("scene_show"), &ev);
		broadcast("show");
		m_app->onScene(this);
	}

	void Scene::hide()
	{
		Rhea::Event ev("scene_hide", this);
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