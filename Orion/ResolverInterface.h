#ifndef ORION_RESOLVER_INTERFACE_H
#define ORION_RESOLVER_INTERFACE_H

#include <string>
#include <memory>
#include "ResourceStream.h"

namespace Orion
{
	class ResolverInterface
	{
	public:
		virtual std::unique_ptr<ResourceStream> getResource(std::string realm_name, std::string resource_path) = 0;
		virtual bool checkResourceExists(std::string realm_name, std::string resource_path) = 0;
	};
}

#endif // !ORION_RESOLVER_INTERFACE_H
