#ifndef RHEA_RESOURCESERVICE_H
#define RHEA_RESOURCESERVICE_H

#include "Service.h"
#include "ResourceList.h"

#include <queue>

namespace Rhea
{
	class ResourceService : public Service
	{
	public:
		ResourceService(std::string name, std::string type_name) :
			Service("ResourceManager", type_name)
		{}

		typedef std::function<void(ResourceList*, Error*)> OnLoadHandler;

		void loadAsync(ResourceList& resreq, OnLoadHandler onLoaded);

		bool onFrame();

		// factory function
		static Unit* createType(std::string name, std::string type);

	protected:
		struct Request {
			ResourceList res_list;
			OnLoadHandler onLoaded;
		};
		std::queue<Request> m_queue;
	};
}

#endif // !RHEA_RESOURCESERVICE_H
