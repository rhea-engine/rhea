#include <Orion/FileAddress.h>

namespace orion
{
    namespace
    {
        int hexValue(unsigned char c)
        {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
            if (c >= 'A' && c <= 'F') return c - 'A' + 10;
            return -1;
        }

        bool isDriveLetter(char c)
        {
            return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
        }

        bool decodeFilePath(const std::string& encoded, std::string& decoded)
        {
            decoded.clear();

            for (size_t i = 0; i < encoded.size(); ++i) {
                unsigned char c = static_cast<unsigned char>(encoded[i]);

                if (c == '%') {
                    if (i + 2 >= encoded.size())
                        return false;

                    const int high = hexValue(encoded[i + 1]);
                    const int low = hexValue(encoded[i + 2]);

                    if (high < 0 || low < 0)
                        return false;

                    c = static_cast<unsigned char>(high * 16 + low);

                    // Encoded separators must not change path structure.
                    if (c == '/')
                        return false;

#if defined(_WIN32)
                    if (c == '\\' || c == ':')
                        return false;
#endif
                    i += 2;
                }

                // Embedded NUL would truncate the filesystem path.
                if (c == 0)
                    return false;

                decoded += static_cast<char>(c);
            }

            return true;
        }
    }

    Error fileAddressToNativePath(const ResourceAddress& address, std::string& native_path)
    {
        native_path.clear();

        if (address.scheme() != AddressScheme::File)
            return Error::DATA_INVALID;

        std::string path;
        if (!decodeFilePath(address.path(), path))
            return Error::DATA_INVALID;

        if (path.empty() || path.front() != '/')
            return Error::DATA_INVALID;

        const std::string host = address.authority();

#if defined(_WIN32)
        if (!host.empty()) {
            // file://server/share/path -> //server/share/path
            const auto share_end = path.find('/', 1);
            const std::string share = path.substr(
                1, share_end == std::string::npos
                ? std::string::npos
                : share_end - 1);

            if (share.empty() || share == "." || share == ".." || host.find_first_of("[]:") != std::string::npos || path.find(':') != std::string::npos) {
                return Error::DATA_INVALID;
            }

            native_path = "//" + host + path;
        }
        else {
            // file:///C:/path -> C:/path
            if (path.size() < 4 ||
                !isDriveLetter(path[1]) ||
                path[2] != ':' ||
                path[3] != '/' ||
                path.find(':', 3) != std::string::npos) {
                return Error::DATA_INVALID;
            }

            native_path = path.substr(1);
        }
#else
        // Remote file authorities require a separate filesystem mapping.
        if (!host.empty())
            return Error::DATA_INVALID;

        native_path = std::move(path);
#endif

        return Error::OK;
    }

    Error resolveNativeResourcePath(const std::string& native_base, const std::string& resource_path, std::string& result)
    {
        result.clear();

        if (native_base.empty() || resource_path.empty() || resource_path.front() != '/' || resource_path.compare(0, 2, "//") == 0) {
            return Error::DATA_INVALID;
        }

        std::string decoded;
        if (!decodeFilePath(resource_path, decoded))
            return Error::DATA_INVALID;

#if defined(_WIN32)
        // These must not introduce native path semantics.
        if (decoded.find_first_of("\\:") != std::string::npos)
            return Error::DATA_INVALID;
#endif

        result = native_base;

        if (result.back() != '/' && result.back() != '\\')
            result += '/';

        result += decoded.substr(1);
        return Error::OK;
    }
}