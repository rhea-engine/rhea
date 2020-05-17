#include "Scene.h"
#include "Item.h"
#include "Graphics/Scene.h"

namespace Rhea
{
	void Scene::addItem(Item* item)
	{
		m_items.push_back(item);
	}	

	void Scene::load(ReadyHandler handler)
	{
		if (m_status >= Status::LOADED)
			handler(ReqError::OK);

		int to_load = (int)m_items.size();
		if (!to_load) {
			m_status = Status::LOADED;
			handler(ReqError::OK);
		}

		// iterate over children and load them
		// call ready handler when all children where loaded	
		// this ignores errors in loading children 
		// TODO check if this should fail fast instead of ignore?
		// Note: handlers and loaders may add items to the list
		int item_size = m_items.size();
		for (int i = 0; i < item_size; i++) {
			Item* item = m_items[i];
			item->load([&](ReqError err) {
				to_load--;
				if (to_load <= 0) {
					m_status = Status::LOADED;
					handler(ReqError::OK);
				}
				});
		}

	}	
}