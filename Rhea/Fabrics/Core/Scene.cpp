#include <memory>

#include "Scene.h"
#include "Fabrics/3D/Item.h"
#include "SceneLoader.h"
#include "EventUnit.h"

namespace Rhea
{
	void Scene::addItem(Item* item)
	{
		m_items.push_back(item);
		sendEvent("add_item", new EventUnit("add_item", item));
	}	

	void Scene::removeItem(Item* item)
	{
		m_items.erase(std::remove(m_items.begin(), m_items.end(), item), m_items.end());
		sendEvent("remove_item", new EventUnit("remove_item", item));
	}

	Unit* Scene::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Scene(name);
		return ret;
	}
}