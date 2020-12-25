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
			m_app(app)
		{};
		~ResourceList();

		void load(Unit::ReadyHandler handler);

		// add a resource info to the list
		void push(std::string key, std::string id, std::string type);

		// get resource with id
		Unit* get(const std::string& key);

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

		std::unordered_map<std::string, ResourceInfo> m_resources;
	};
}

#endif // !RHEA_RESOURCELIST_H
