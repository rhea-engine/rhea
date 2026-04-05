#include <Orion/ResourceCache.h>

namespace orion
{
	ResourceCache::ResourceCache()
	{

	}

	std::unique_ptr<ResourceStream> ResourceCache::getResource(std::string realm_name, std::string resource_path)
	{
		return nullptr;
	}

	bool ResourceCache::checkResourceExists(std::string realm_name, std::string resource_path)
	{
		return false;
	}
}