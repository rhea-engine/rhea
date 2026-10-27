/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ORION_RESOURCECACHE_H
#define ORION_RESOURCECACHE_H

#include <Orion/ResolverInterface.h>

#include <Orion/Orion.h>

namespace orion
{
	class ResourceCache : public ResolverInterface
	{
	public:
		ResourceCache();

		std::unique_ptr<ResourceStream> getResource(std::string realm_name, std::string resource_path);
		bool checkResourceExists(std::string realm_name, std::string resource_path);

		std::string getBasicAddress() const override;
	};
}

#endif // !ORION_RESOURCECACHE_H
