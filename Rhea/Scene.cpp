#include <memory>

#include "Scene.h"
#include "Item.h"
#include "SceneLoader.h"
#include "EventUnit.h"

namespace Rhea
{
	void Scene::addItem(Item* item)
	{
		m_items.push_back(item);
		sendEvent("add_item", new EventUnit("add_item", item));
	}	

	Unit* Scene::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Scene(name);
		return ret;
	}
}