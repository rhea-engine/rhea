/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Resources/ResourceList.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Application.h>
#include <Psio/Log.h>

#include <cassert>

namespace rhea {

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
		m_to_load = (int)item_size;
		m_load_errors = ErrorReport();
		if (!item_size) {
			// no external resources -> immediately succeed
			m_loaded = true;
			handler(m_load_errors);
			return;
		}
		UnitTree* tree = m_app->getUnitTree();
		for (auto it = m_resources.begin(); it != m_resources.end(); ++it)
		{
			ResourceInfo res = it->second;
			std::string key = it->first;
			if (res.status == Resource::Status::LOADED) {
				// already loaded
				decrementToLoadAndCheckIfZero(handler, nullptr);
			}
			else {
				Unit* resource = res.resource ? res.resource : doCast<Unit>(tree->find(m_app->resourceUriToId(res.id)));
				if (resource) {
					// exists in tree					
					m_resources[key].status = Resource::Status::LOADED;
					m_resources[key].resource = resource;
					resource->incRef();
					decrementToLoadAndCheckIfZero(handler, nullptr);
				}
				else if (res.resource) {
					// exists in res list						
					m_resources[key].status = Resource::Status::LOADED;
					m_resources[key].resource = res.resource;
					decrementToLoadAndCheckIfZero(handler, nullptr);
				}
				else {
					m_app->loadResource(res.id, m_resource_context, res.type, [&, res, key, handler](Component* component, ErrorReport err) {
						Unit* resource = doCast<Unit>(component);
						if (resource && !err.isError()) {
							m_resources[key].status = Resource::Status::LOADED;
							m_resources[key].resource = resource;
							resource->incRef();
							decrementToLoadAndCheckIfZero(handler, &err);
						}
						else {
							if (!err.isError())
								err.addError(Error(Error::Code::INVALID_DATA, "Loaded component is not a resource unit.", res.id));
							LOG(LOG_ERROR) << "Could not load resource " << res.id;
							m_resources[key].status = Resource::Status::DATA_INVALID;
							decrementToLoadAndCheckIfZero(handler, &err);

						}
						});
				}
			}
		}
	}

	bool ResourceList::decrementToLoadAndCheckIfZero(Unit::ReadyHandler handler, ErrorReport* err) {
		if (err) {
			m_load_errors.merge(*err);
		}
		if (--m_to_load != 0)
			return false;

		m_loaded = true;
		if (handler)
			handler(m_load_errors);

		return true;
	}
	
	void ResourceList::push(std::string key, std::string id, std::string type, bool replace, Unit* unit)
	{
		assert(id.compare("No type"));

		// only add new resources
		if (m_resources.find(key) == m_resources.end()) {
			m_resources[key] = {
				id,
				type,
				Resource::Status::SETUP,
				unit
			};
		}
		else if (replace) {
			// if replace option was set - replace resources that where not loaded yet
			if (m_resources[key].status == Resource::Status::SETUP) {
				m_resources[key].id = id;
				m_resources[key].type = type;
				m_resources[key].resource = unit;
			}
			else {
				LOG(LOG_ERROR) << "ResourceList: Could not replace resource '" << key << "': already loaded";
			}
		}
		else {
			LOG(LOG_ERROR) << "ResourceList: resource '" << key << "' already exists and replace was false";
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
