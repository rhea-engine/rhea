#include "Scene.h"
#include <memory>

#include "Scene.h"
#include "Fabrics/3D/Item.h"
#include "SceneLoader.h"
#include "Event.h"

namespace Rhea
{
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
			Unit* child = it->second;
			if (child->getValue("isItem"))
				m_items.push_back((Item*)child);
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

	Unit* Scene::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Scene(name);
		return ret;
	}
}