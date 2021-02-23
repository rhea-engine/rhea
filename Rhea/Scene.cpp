#include <memory>

#include "Scene.h"
#include "Item.h"
#include "SceneLoader.h"

namespace Rhea
{
	void Scene::addItem(Item* item)
	{
		m_items.push_back(item);
	}	

	Unit* Scene::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Scene(name);
		return ret;
	}

	void Scene::load(Orion::ResourceRequest* req, ReadyHandler handler)
	{
		// for now we assume a scene file
		std::unique_ptr<SceneLoader> parser = std::make_unique<SceneLoader>();

		// parse the file
		parser->load(this, req->getResource(), m_app);

		// assume everything is fine
		handler(ReqError::OK);
	}
}