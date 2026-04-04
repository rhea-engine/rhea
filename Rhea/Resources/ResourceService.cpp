#include <Rhea/Resources/ResourceService.h>

namespace rhea {

	void ResourceService::loadAsync(ResourceList& resreq, OnLoadHandler onLoaded)
	{
		if (resreq.size()) {
			std::shared_ptr<ResourceList> rreq = std::make_shared<ResourceList>(resreq);

			rreq->load([this, rreq, onLoaded](rhea::Error* err) {
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
}