#include "ResourceList.h"
#include "UnitTree.h"
#include "Application.h"

namespace Rhea {
	
	ResourceList::~ResourceList()
	{
		// decrement all references on loaded resources
		for (auto it = m_resources.begin(); it != m_resources.end(); ++it)
		{
			ResourceInfo res_info = it->second;
			if (res_info.status == Resource::Status::LOADED)
				res_info.resource->decRef();
		}
	}

	void ResourceList::load(Unit::ReadyHandler handler)
	{
		// iterate over resource list and load them
		// call ready handler when all resources where loaded	
		// this ignores errors in loading resources
		// TODO check if this should fail fast instead of ignore?

		size_t item_size = m_resources.size();
		m_to_load = item_size;
		if (!item_size) {
			// no external resources -> immediately succeed
			handler(nullptr);
			return;
		}
		UnitTree* tree = m_app->getUnitTree();
		for (auto it = m_resources.begin(); it != m_resources.end(); ++it)
		{
			ResourceInfo res = it->second;
			std::string key = it->first;
			if (res.status == Resource::Status::LOADED) {
				// already loaded
				m_to_load--;
				if (m_to_load == 0) {
					handler(nullptr);
				}
			}
			else {
				Unit* resource = tree->find(res.id);
				if (resource) {
					// exists in tree
					resource->prepare([&](Error* err) {
						if (err) {
							m_app->log("Could not load resource '" + res.id + "'");
						}
						m_to_load--;
						m_resources[key].status = Resource::Status::LOADED;
						m_resources[key].resource = resource;
						resource->incRef();
						if (m_to_load == 0) {
							handler(nullptr);
						}
						});
				}
				else {
					m_app->loadResource(res.id, res.type, [&, res, key, handler](Unit* resource, Orion::ResourceRequest::Error err) {
						if (resource) {
							if (err != Orion::ResourceRequest::Error::OK) {
								m_app->log("Could not load resource " + res.id + ": " + Orion::ResourceRequest::getErrorMessage(err));
							}
							resource->prepare([&](Error* err) {
								if (err) {
									m_app->log("Could not load resource '" + res.id + "'");
								}
								m_to_load--;
								m_resources[key].status = Resource::Status::LOADED;
								m_resources[key].resource = resource;
								resource->incRef();
								if (m_to_load == 0) {
									handler(nullptr);
								}
								});
						}
						else {
							m_app->log("Could not load resource " + res.id + ": " + Orion::ResourceRequest::getErrorMessage(err));
							m_resources[key].status = Resource::Status::DATA_INVALID;
							m_to_load--;
							if (m_to_load == 0) {
								handler(nullptr);
							}
						}
						});
				}
			}
		}
	}
	
	void ResourceList::push(std::string key, std::string id, std::string type, bool replace)
	{
		// only add new resources
		if (m_resources.find(key) == m_resources.end()) {
			m_resources[key] = {
				id,
				type,
				Resource::Status::SETUP,
				nullptr
			};
		}
		else if (replace) {
			// if replace option was set - replace resources that where not loaded yet
			if (m_resources[key].status == Resource::Status::SETUP) {
				m_resources[key].id = id;
				m_resources[key].type = type;
			}
			else {
				m_app->log("ResourceList: Could not replace resource '" + key + "': already loaded");
			}
		}
		else {
			m_app->log("ResourceList: resource '" + key + "' already exists and replace was false");
		}
	}

	Unit* ResourceList::get(const std::string& key)
	{
		auto res_info_it = m_resources.find(key);
		if (res_info_it != m_resources.end()) {
			ResourceInfo res_info = res_info_it->second;
			if (res_info.status == Resource::Status::LOADED)
				return res_info.resource;
		}
		return nullptr;
	}
	
	void ResourceList::release(const std::string& key)
	{
		// search for resource and decrease ref count then remove from list
		auto res_info_it = m_resources.find(key);
		if (res_info_it != m_resources.end()) {
			ResourceInfo res_info = res_info_it->second;
			if (res_info.status == Resource::Status::LOADED)
				res_info.resource->decRef();
			m_resources.erase(res_info_it);
		}
	}
}
