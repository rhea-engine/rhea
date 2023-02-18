#ifndef RHEA_RESOURCELIST_H
#define RHEA_RESOURCELIST_H

#include <unordered_map>

#include "Resource.h"

namespace Rhea
{
	class Application;

	/** a class for holding a list of resources and loading them. used by units.
	*/
	class ResourceList
	{
	public:
		ResourceList(Application* app) :
			m_app(app),
			m_to_load(0)
		{};
		~ResourceList();

		void load(Unit::ReadyHandler handler);

		// add a resource info to the list
		void push(std::string key, std::string id, std::string type, bool replace = false);

		// get resource with id
		Unit* get(const std::string& key);

		// Access by []
		Unit* operator[](std::string key) { return get(key); }
		Unit* operator[](const char* key) { return get(key); }

		// release a resource
		void release(const std::string& key);

	protected:
		struct ResourceInfo {
			std::string id;
			std::string type;
			Resource::Status status;
			Unit* resource;
		};
		Application* m_app;
		int m_to_load;

		std::unordered_map<std::string, ResourceInfo> m_resources;
	};
}

#endif // !RHEA_RESOURCELIST_H
