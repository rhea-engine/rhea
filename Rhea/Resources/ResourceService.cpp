#include "ResourceService.h"

namespace Rhea {

	void ResourceService::loadAsync(ResourceList& resreq, OnLoadHandler onLoaded)
	{
		if (resreq.size()) {
			std::shared_ptr<ResourceList> rreq = std::make_shared<ResourceList>(resreq);

			rreq->load([this, rreq, onLoaded](Rhea::Error* err) {
				if (onLoaded) {
					if (!err) {
						onLoaded(rreq.get(), nullptr);
					}
					else {
						onLoaded(rreq.get(), err);
					}
				}
				});
		}
		else {
			onLoaded(&resreq, nullptr);
		}
	}

	Unit* ResourceService::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new ResourceService(name, type);
		return ret;
	}

}