/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Resources/ResourceService.h>

namespace rhea {

	void ResourceService::loadAsync(ResourceList& resreq, OnLoadHandler onLoaded)
	{
		if (resreq.size()) {
			std::shared_ptr<ResourceList> rreq = std::make_shared<ResourceList>(resreq);

			rreq->load([this, rreq, onLoaded](rhea::ErrorReport err) {
				if (onLoaded) {
					onLoaded(rreq.get(), err);
				}
				});
		}
		else {
			onLoaded(&resreq, ErrorReport());
		}
	}
}