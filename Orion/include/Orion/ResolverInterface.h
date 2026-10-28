/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ORION_RESOLVER_INTERFACE_H
#define ORION_RESOLVER_INTERFACE_H

#include <string>
#include <memory>
#include <Orion/ResourceStream.h>

namespace orion
{
	class ResolverInterface
	{
	public:
		virtual std::unique_ptr<ResourceStream> getResource(std::string realm_name, std::string resource_path) = 0;
		virtual bool checkResourceExists(std::string realm_name, std::string resource_path) = 0;
		virtual std::string getBasicAddress() const = 0;
	};
}

#endif // !ORION_RESOLVER_INTERFACE_H
