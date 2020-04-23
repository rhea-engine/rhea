#ifndef ORION_RESOURCECACHE_H
#define ORION_RESOURCECACHE_H

#include <string>

using std::string;

#include "Orion.h"

namespace Orion
{
	class ResourceReadyInterface;

	class ResourceCache
	{
	public:
		ResourceCache();
		int GetResource(string url);
	};
}

#endif // !ORION_RESOURCECACHE_H
