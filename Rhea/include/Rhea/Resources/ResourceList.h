/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_RESOURCELIST_H
#define RHEA_RESOURCELIST_H

#include <unordered_map>

#include "Resource.h"

namespace rhea
{
	class Application;

	/** a class for holding a list of resources and loading them. used by units.
	*/
	class ResourceList
	{
	public:
		struct ResourceInfo {
			std::string id;
			std::string type;
			Resource::Status status;
			Unit* resource;
		};

		ResourceList(Application* app) :
			m_app(app),
			m_to_load(0),
			m_loaded(false)
		{};
		~ResourceList();

		void load(Unit::ReadyHandler handler);

		bool decrementToLoadAndCheckIfZero(Unit::ReadyHandler handler, Error* err);

		// add a resource info to the list
		void push(std::string key, std::string id, std::string type, bool replace = false, Unit* unit = nullptr);

		// get resource with id
		Unit* get(const std::string& key);

		// Access by []
		Unit* operator[](std::string key) { return get(key); }
		Unit* operator[](const char* key) { return get(key); }

		// release a resource
		void release(const std::string& key);

		bool isLoaded() { return m_loaded; }

		size_t size() { return m_resources.size(); }

		const std::unordered_map<std::string, ResourceInfo>& getAllComponents() const { return m_resources; }

	protected:
		Application* m_app;
		int m_to_load;
		bool m_loaded;

		std::unordered_map<std::string, ResourceInfo> m_resources;
	};
}

#endif // !RHEA_RESOURCELIST_H
