/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_RESOURCESERVICE_H
#define RHEA_RESOURCESERVICE_H

#include <Rhea/Service.h>
#include <Rhea/Resources/ResourceList.h>
#include <queue>

namespace rhea
{
	class ResourceService : public Service
	{
		MECLASS(ResourceService, Service)
	public:
		ResourceService(std::string name, std::string type_name) :
			Service("ResourceManager", type_name)
		{}

		typedef std::function<void(ResourceList*, ErrorReport)> OnLoadHandler;

		void loadAsync(ResourceList& resreq, OnLoadHandler onLoaded);

	protected:
		struct Request {
			ResourceList res_list;
			OnLoadHandler onLoaded;
		};
		std::queue<Request> m_queue;
	};
}

#endif // !RHEA_RESOURCESERVICE_H
