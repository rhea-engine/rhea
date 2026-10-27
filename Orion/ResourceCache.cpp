/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

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

	std::string ResourceCache::getBasicAddress() const
	{
		return "orion://resourcecache";
	}
}