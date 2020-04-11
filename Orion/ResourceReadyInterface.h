#ifndef ORION_RESOURCEREADYINTERFACE_H
#define ORION_RESOURCEREADYINTERFACE_H

#include "ResourceRequest.h"

namespace Orion
{
	class Resource;

	class ResourceReadyInterface
	{
	public:
		virtual void onResourceReady(Resource*) = 0;
		virtual void onResourceFailed(ResourceRequest::Error reason) = 0;
	};
}

#endif // !ORION_RESOURCEREADYINTERFACE_H
