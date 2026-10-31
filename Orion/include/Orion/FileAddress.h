#ifndef ORION_FILE_ADDRESS_H
#define ORION_FILE_ADDRESS_H

#include <Orion/ResourceAddress.h>
#include <Orion/RequestError.h>
#include <string>

namespace orion
{
    Error fileAddressToNativePath(const ResourceAddress& address, std::string& native_path);
	Error resolveNativeResourcePath(const std::string& native_base, const std::string& resource_path, std::string& result);
}

#endif // !ORION_FILE_ADDRESS_H