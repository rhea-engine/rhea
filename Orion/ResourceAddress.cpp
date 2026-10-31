#include <Orion/ResourceAddress.h>

#include <algorithm>
#include <vector>

namespace orion
{
    namespace
    {
        bool isAlpha(unsigned char c) {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
        }

        bool isDigit(unsigned char c) { return c >= '0' && c <= '9'; }

        bool isUnreserved(unsigned char c) {
            return isAlpha(c) || isDigit(c) || c == '-' || c == '.' || c == '_' || c == '~';
        }

        int hexValue(unsigned char c) {
            if (isDigit(c)) return c - '0';
            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
            if (c >= 'A' && c <= 'F') return c - 'A' + 10;
            return -1;
        }

        std::string lowerAscii(std::string text) {
            for (char& c : text)
                if (c >= 'A' && c <= 'Z') c = static_cast<char>(c + ('a' - 'A'));
            return text;
        }

        AddressResult failure(AddressErrorCode code, std::string message, size_t position) {
            AddressResult result;
            result.error = { code, std::move(message), position };
            return result;
        }

        const char* schemeName(AddressScheme scheme) {
            switch (scheme) {
            case AddressScheme::File: return "file";
            case AddressScheme::Http: return "http";
            case AddressScheme::Https: return "https";
            case AddressScheme::Realm: return "realm";
            }
            return "";
        }

        // Accept valid UTF-8 input; URI output encodes its bytes without Unicode normalization.
        bool validUtf8(std::string_view text, size_t& invalid_position) {
            for (size_t i = 0; i < text.size();) {
                const auto c = static_cast<unsigned char>(text[i]);
                size_t count = 0;
                unsigned int value = 0;
                unsigned int minimum = 0;
                if (c < 0x80) { ++i; continue; }
                if (c >= 0xC2 && c <= 0xDF) { count = 2; value = c & 0x1F; minimum = 0x80; }
                else if (c >= 0xE0 && c <= 0xEF) { count = 3; value = c & 0x0F; minimum = 0x800; }
                else if (c >= 0xF0 && c <= 0xF4) { count = 4; value = c & 0x07; minimum = 0x10000; }
                else { invalid_position = i; return false; }
                if (count > text.size() - i) { invalid_position = i; return false; }
                for (size_t j = 1; j < count; ++j) {
                    const auto next = static_cast<unsigned char>(text[i + j]);
                    if ((next & 0xC0) != 0x80) { invalid_position = i + j; return false; }
                    value = (value << 6) | (next & 0x3F);
                }
                if (value < minimum || value > 0x10FFFF || (value >= 0xD800 && value <= 0xDFFF)) {
                    invalid_position = i;
                    return false;
                }
                i += count;
            }
            return true;
        }

        bool encodeComponent(std::string_view input, bool query, size_t offset,
            std::string& output, AddressError& error) {
            constexpr char hex[] = "0123456789ABCDEF";
            for (size_t i = 0; i < input.size(); ++i) {
                const auto c = static_cast<unsigned char>(input[i]);
                if (c == '%') {
                    if (i + 2 >= input.size() || hexValue(input[i + 1]) < 0 || hexValue(input[i + 2]) < 0) {
                        error = { AddressErrorCode::InvalidEncoding, "Invalid percent escape", offset + i };
                        return false;
                    }
                    const auto decoded = static_cast<unsigned char>(
                        hexValue(input[i + 1]) * 16 + hexValue(input[i + 2]));
                    // Decode unreserved characters, but never encoded separators.
                    if (isUnreserved(decoded)) output += static_cast<char>(decoded);
                    else {
                        output += '%';
                        output += hex[decoded >> 4];
                        output += hex[decoded & 15];
                    }
                    i += 2;
                }
                else if (isUnreserved(c) || std::string_view("!$&'()*+,;=:@/").find(c) != std::string_view::npos
                    || (query && c == '?')) output += static_cast<char>(c);
                else {
                    output += '%';
                    output += hex[c >> 4];
                    output += hex[c & 15];
                }
            }
            return true;
        }

        bool validRealm(std::string_view name) {
            if (name.empty() || name.size() > 63) return false;
            const auto alnum = [](unsigned char c) { return isAlpha(c) || isDigit(c); };
            return alnum(name.front()) && alnum(name.back()) &&
                std::all_of(name.begin(), name.end(), [&](unsigned char c) { return alnum(c) || c == '-'; });
        }

        bool validIpv4(std::string_view host) {
            size_t start = 0;
            int parts = 0;
            while (start < host.size()) {
                size_t end = host.find('.', start);
                if (end == std::string_view::npos) end = host.size();
                if (end == start || end - start > 3 || (end - start > 1 && host[start] == '0')) return false;
                unsigned int value = 0;
                for (size_t i = start; i < end; ++i) {
                    if (!isDigit(host[i])) return false;
                    value = value * 10 + host[i] - '0';
                }
                if (value > 255) return false;
                ++parts;
                if (end == host.size()) return parts == 4;
                start = end + 1;
            }
            return false;
        }

        bool validIpv6(std::string_view host) {
            const size_t compression = host.find("::");
            if (compression != std::string_view::npos && host.find("::", compression + 2) != std::string_view::npos)
                return false;
            const auto countGroups = [](std::string_view side, bool allow_ipv4, int& groups) {
                if (side.empty()) return true;
                size_t start = 0;
                while (start < side.size()) {
                    size_t end = side.find(':', start);
                    if (end == std::string_view::npos) end = side.size();
                    const auto group = side.substr(start, end - start);
                    if (group.empty()) return false;
                    if (group.find('.') != std::string_view::npos) {
                        if (!allow_ipv4 || end != side.size() || !validIpv4(group)) return false;
                        groups += 2;
                    }
                    else {
                        if (group.size() > 4 || !std::all_of(group.begin(), group.end(),
                            [](unsigned char c) { return hexValue(c) >= 0; })) return false;
                        ++groups;
                    }
                    if (end == side.size()) return true;
                    start = end + 1;
                }
                return false;
                };
            int groups = 0;
            if (compression == std::string_view::npos) return countGroups(host, true, groups) && groups == 8;
            return countGroups(host.substr(0, compression), false, groups) &&
                countGroups(host.substr(compression + 2), true, groups) && groups < 8;
        }

        // V1 supports ASCII DNS names (including punycode), IPv4, and bracketed IPv6.
        // Unicode domain names require an IDNA adapter before this parser.
        bool validHost(std::string_view host) {
            if (host.empty()) return false;
            if (host.front() == '[')
                return host.size() > 2 && host.back() == ']' && validIpv6(host.substr(1, host.size() - 2));
            if (host.back() == '.') host.remove_suffix(1);
            if (host.empty() || host.size() > 253) return false;
            size_t start = 0;
            while (start < host.size()) {
                size_t end = host.find('.', start);
                if (end == std::string_view::npos) end = host.size();
                if (!validRealm(host.substr(start, end - start))) return false;
                start = end + 1;
            }
            return true;
        }

        size_t rootDepth(AddressScheme scheme, std::string_view authority, std::string_view path) {
            if (scheme != AddressScheme::File) return 0;
            if (!authority.empty()) return 1; // UNC share.
            if (path.size() >= 3 && isAlpha(path[1]) && path[2] == ':') return 1; // Drive.
            return 0;
        }

        // Preserve empty segments: /a//b and /a/b may be different HTTP resources.
        bool normalizePath(std::string_view path, size_t root_depth,
            std::string& normalized, size_t& escape_position) {
            std::vector<std::string> segments;
            size_t start = 1;
            while (start < path.size()) {
                size_t end = path.find('/', start);
                if (end == std::string_view::npos) end = path.size();
                const auto segment = path.substr(start, end - start);
                if (segment == "..") {
                    if (segments.size() > root_depth) segments.pop_back();
                    else {
                        escape_position = start;
                        return false;
                    }
                }
                else if (segment != ".") segments.emplace_back(segment);
                start = end + 1;
            }
            std::string result = "/";
            for (size_t i = 0; i < segments.size(); ++i) {
                if (i) result += '/';
                result += segments[i];
            }
            const bool trailing = path.back() == '/' || path == "/." || path == "/.." ||
                (path.size() >= 2 && path.substr(path.size() - 2) == "/.") ||
                (path.size() >= 3 && path.substr(path.size() - 3) == "/..");
            if (path.back() == '/' && !segments.empty()) result += '/';
            else if (trailing && result.back() != '/') result += '/';
            if (root_depth && segments.size() == root_depth && result.back() != '/') result += '/';
            normalized = std::move(result);
            return true;
        }
    }

    AddressResult ResourceAddress::parse(std::string_view text) {
        const size_t colon = text.find(':');
        if (colon == std::string_view::npos)
            return failure(AddressErrorCode::InvalidSyntax, "An absolute address requires a scheme", 0);
        const std::string scheme = lowerAscii(std::string(text.substr(0, colon)));
        if (scheme.empty() || !isAlpha(scheme.front()) || !std::all_of(scheme.begin(), scheme.end(),
            [](unsigned char c) { return isAlpha(c) || isDigit(c) || c == '+' || c == '-' || c == '.'; }))
            return failure(AddressErrorCode::InvalidSyntax, "Invalid scheme", 0);

        AddressResult result;
        auto& address = result.address;
        if (scheme == "file") address.m_scheme = AddressScheme::File;
        else if (scheme == "http") address.m_scheme = AddressScheme::Http;
        else if (scheme == "https") address.m_scheme = AddressScheme::Https;
        else if (scheme == "realm") address.m_scheme = AddressScheme::Realm;
        else return failure(AddressErrorCode::UnsupportedScheme, "Unsupported scheme", 0);

        if (text.substr(colon + 1, 2) != "//")
            return failure(AddressErrorCode::InvalidSyntax, "Expected ://", colon + 1);
        size_t bad_utf8 = 0;
        if (!validUtf8(text, bad_utf8))
            return failure(AddressErrorCode::InvalidEncoding, "Invalid UTF-8 input", bad_utf8);
        for (size_t i = 0; i < text.size(); ++i)
            if (static_cast<unsigned char>(text[i]) < 0x20 || text[i] == 0x7F || text[i] == '\\')
                return failure(AddressErrorCode::InvalidSyntax, "Control characters and backslashes are not allowed", i);
        const size_t fragment = text.find('#');
        if (fragment != std::string_view::npos && fragment + 1 < text.size())
            return failure(AddressErrorCode::UnsupportedFragment, "Nonempty fragments are not supported", fragment);
        if (fragment != std::string_view::npos) text = text.substr(0, fragment);

        const size_t authority_start = colon + 3;
        size_t authority_end = text.find_first_of("/?", authority_start);
        if (authority_end == std::string_view::npos) authority_end = text.size();
        address.m_authority = lowerAscii(std::string(text.substr(authority_start, authority_end - authority_start)));
        if (address.m_scheme == AddressScheme::Realm) {
            if (!validRealm(address.m_authority) || address.m_authority == "file" ||
                address.m_authority == "recent" || address.m_authority == "realms")
                return failure(AddressErrorCode::InvalidRealmName, "Invalid or reserved realm name", authority_start);
        }
        else {
            std::string_view authority = address.m_authority;
            std::string_view host = authority;
            size_t port_start = std::string_view::npos;
            if (!authority.empty() && authority.front() == '[') {
                const auto bracket = authority.find(']');
                if (bracket != std::string_view::npos) {
                    host = authority.substr(0, bracket + 1);
                    if (bracket + 1 < authority.size()) {
                        if (authority[bracket + 1] != ':')
                            return failure(AddressErrorCode::InvalidAuthority, "Invalid IPv6 authority", authority_start);
                        port_start = bracket + 2;
                    }
                }
            }
            else {
                const auto separator = authority.find(':');
                if (separator != std::string_view::npos) { host = authority.substr(0, separator); port_start = separator + 1; }
            }
            const bool file = address.m_scheme == AddressScheme::File;
            if ((!host.empty() && !validHost(host)) || (!file && host.empty()) ||
                (file && port_start != std::string_view::npos))
                return failure(AddressErrorCode::InvalidAuthority, "Invalid host, port, or user information", authority_start);
            if (port_start != std::string_view::npos) {
                const auto port = authority.substr(port_start);
                unsigned int number = 0;
                if (port.empty()) return failure(AddressErrorCode::InvalidAuthority, "Empty port", authority_start + port_start);
                for (size_t i = 0; i < port.size(); ++i) {
                    if (!isDigit(port[i]) || number > 6553 || (number == 6553 && port[i] > '5'))
                        return failure(AddressErrorCode::InvalidAuthority, "Invalid port", authority_start + port_start + i);
                    number = number * 10 + port[i] - '0';
                }
            }
            if (file && address.m_authority == "localhost") address.m_authority.clear();
        }

        const size_t query = text.find('?', authority_end);
        address.m_has_query = query != std::string_view::npos;
        if (address.m_has_query && address.m_scheme != AddressScheme::Http && address.m_scheme != AddressScheme::Https)
            return failure(AddressErrorCode::UnsupportedQuery, "Query is only supported for HTTP(S)", query);
        auto path = text.substr(authority_end, (address.m_has_query ? query : text.size()) - authority_end);
        if (path.empty()) path = "/";
        if (!encodeComponent(path, false, authority_end, address.m_path, result.error)) return result;
        if (address.m_has_query && !encodeComponent(text.substr(query + 1), true, query + 1, address.m_query, result.error))
            return result;
        const size_t root_depth = rootDepth(address.m_scheme, address.m_authority, address.m_path);
        if (address.m_scheme == AddressScheme::File) {
            if (!address.m_authority.empty() && (address.m_path.size() == 1 || address.m_path[1] == '/'))
                return failure(AddressErrorCode::InvalidSyntax, "A file host requires a share path", authority_end);
            if (address.m_authority.empty() && address.m_path.size() >= 3 && address.m_path[2] == ':' &&
                (address.m_path.size() < 4 || address.m_path[3] != '/'))
                return failure(AddressErrorCode::InvalidSyntax, "A drive requires an absolute path", authority_end);
        }
        size_t escape_position = 0;
        std::string normalized_path;
        if (!normalizePath(address.m_path, root_depth, normalized_path, escape_position))
            return failure(AddressErrorCode::InvalidSyntax, "Parent path segment escapes root",
                authority_end + escape_position);
        address.m_path = std::move(normalized_path);
        result.success = true;
        return result;
    }

    AddressScheme ResourceAddress::scheme() const { return m_scheme; }

    std::string ResourceAddress::toString() const {
        return std::string(schemeName(m_scheme)) + "://" + m_authority + m_path +
            (m_has_query ? "?" + m_query : "");
    }

    AddressResult ResourceAddress::parent() const {
        ResourceAddress address = *this;
        address.m_query.clear();
        address.m_has_query = false;
        const size_t root_depth = rootDepth(m_scheme, m_authority, m_path);
        size_t root_end = 1;
        if (root_depth) {
            root_end = m_path.find('/', 1);
            root_end = root_end == std::string::npos ? m_path.size() : root_end + 1;
        }
        if (address.m_path.size() > root_end) {
            if (address.m_path.back() == '/') address.m_path.pop_back();
            address.m_path.resize(address.m_path.find_last_of('/') + 1);
        }
        AddressResult result;
        result.success = true;
        result.address = std::move(address);
        return result;
    }

    AddressResult ResourceAddress::resolveRelative(std::string_view reference) const {
        // This is URI resolution, not the Ceto //realm shorthand or Rhea project-root semantics.
        const size_t colon = reference.find(':');
        const size_t delimiter = reference.find_first_of("/?#");
        if (colon != std::string_view::npos && (delimiter == std::string_view::npos || colon < delimiter))
            return parse(reference);
        if (reference.substr(0, 2) == "//") {
            const std::string scheme_prefix = std::string(schemeName(m_scheme)) + ":";
            auto result = parse(scheme_prefix + std::string(reference));
            if (!result.success)
                result.error.position = result.error.position >= scheme_prefix.size() ?
                result.error.position - scheme_prefix.size() : 0;
            return result;
        }

        const std::string prefix = std::string(schemeName(m_scheme)) + "://" + m_authority;
        std::string candidate;
        size_t reference_offset = 0;
        if (reference.empty() || reference.front() == '#') {
            candidate = toString();
            reference_offset = candidate.size();
        }
        else if (reference.front() == '?') {
            candidate = prefix + m_path;
            reference_offset = candidate.size();
        }
        else if (reference.front() == '/') {
            candidate = prefix;
            reference_offset = candidate.size();
        }
        else {
            candidate = prefix + m_path.substr(0, m_path.find_last_of('/') + 1);
            reference_offset = candidate.size();
        }
        candidate += reference;
        auto result = parse(candidate);
        if (!result.success)
            result.error.position = result.error.position >= reference_offset ? result.error.position - reference_offset : 0;
        return result;
    }

    bool ResourceAddress::operator==(const ResourceAddress& other) const {
        return m_scheme == other.m_scheme && m_authority == other.m_authority && m_path == other.m_path &&
            m_has_query == other.m_has_query && m_query == other.m_query;
    }
}
