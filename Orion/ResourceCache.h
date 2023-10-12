#ifndef ORION_RESOURCECACHE_H
#define ORION_RESOURCECACHE_H

#include "ResolverInterface.h"

#include "Orion.h"

namespace Orion
{
	class ResourceCache : public ResolverInterface
	{
	public:
		ResourceCache();

		std::unique_ptr<ResourceStream> getResource(std::string realm_name, std::string resource_path);
		bool checkResourceExists(std::string realm_name, std::string resource_path);
	};
}

#endif // !ORION_RESOURCECACHE_H
