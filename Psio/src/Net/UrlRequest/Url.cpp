/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Psio/Net/UrlRequest/Url.h>
#include <algorithm>
#include <cctype>
#include <string>

namespace psio
{
	struct UriParts {
		std::string scheme;
		std::string authority;
		std::string path;
		std::string query;
		std::string fragment;

		bool has_scheme = false;
		bool has_authority = false;
		bool has_query = false;
		bool has_fragment = false;
	};

	bool isAlpha(unsigned char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}

	bool isDigit(unsigned char c) { return c >= '0' && c <= '9'; }

	int hexValue(unsigned char c) {
		if (isDigit(c)) return c - '0';
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		if (c >= 'A' && c <= 'F') return c - 'A' + 10;
		return -1;
	}

	std::string removeDotSegments(std::string input)
	{
		std::string output;

		auto removeLastSegment = [&]() {
			const auto pos = output.find_last_of('/');
			if (pos == std::string::npos)
				output.clear();
			else
				output.erase(pos);
			};

		while (!input.empty()) {
			if (input.compare(0, 3, "../") == 0) {
				input.erase(0, 3);
			}
			else if (input.compare(0, 2, "./") == 0) {
				input.erase(0, 2);
			}
			else if (input.compare(0, 3, "/./") == 0) {
				input.erase(0, 2);
			}
			else if (input == "/.") {
				input = "/";
			}
			else if (input.compare(0, 4, "/../") == 0) {
				input.erase(0, 3);
				removeLastSegment();
			}
			else if (input == "/..") {
				input = "/";
				removeLastSegment();
			}
			else if (input == "." || input == "..") {
				input.clear();
			}
			else {
				// Nächstes Segment einschließlich führendem Slash
				// von input nach output verschieben.
				const auto end = input.find(
					'/', input.front() == '/' ? 1 : 0);

				if (end == std::string::npos) {
					output += input;
					input.clear();
				}
				else {
					output += input.substr(0, end);
					input.erase(0, end);
				}
			}
		}

		return output;
	}

	UriParts parseUri(const std::string& uri)
	{
		UriParts parts;
		if (uri.empty())
			return parts;

		// fragment: find '#' first
		size_t fragment_pos = uri.find('#');
		if (fragment_pos != std::string::npos) {
			parts.fragment = uri.substr(fragment_pos + 1);
			parts.has_fragment = true;
		}
		std::string uri_without_fragment = (fragment_pos != std::string::npos) ? uri.substr(0, fragment_pos) : uri;

		// query: find '?' next
		size_t query_pos = uri_without_fragment.find('?');
		if (query_pos != std::string::npos) {
			parts.query = uri_without_fragment.substr(query_pos + 1);
			parts.has_query = true;
		}
		std::string uri_without_query = (query_pos != std::string::npos) ? uri_without_fragment.substr(0, query_pos) : uri_without_fragment;

		// find scheme
		size_t scheme_end = 0;
		for (size_t i = 0; i < uri_without_query.size(); ++i) {
			char c = uri_without_query[i];
			if (i == 0 && !std::isalpha(static_cast<unsigned char>(c))) {
				// scheme must start with a letter
				scheme_end = 0;
				break;
			}
			else if (c == ':') {
				scheme_end = i;
				break;
			}
			else if (!isAlpha(static_cast<unsigned char>(c)) && !isDigit(static_cast<unsigned char>(c)) && c != '-' && c != '.') {
				// invalid scheme character
				scheme_end = 0;
				break;
			}
		}

		if (scheme_end != 0) {
			parts.scheme = uri_without_query.substr(0, scheme_end);
			parts.has_scheme = true;
		}
		std::string uri_without_scheme = (scheme_end != 0) ? uri_without_query.substr(scheme_end + 1) : uri_without_query;

		// find authority and path		
		if (uri_without_scheme.substr(0, 2) == "//") {
			size_t authority_start = 2;
			size_t path_start = uri_without_scheme.find('/', authority_start);
			if (path_start != std::string::npos) {
				parts.authority = uri_without_scheme.substr(authority_start, path_start - authority_start);
				parts.has_authority = true;
				parts.path = uri_without_scheme.substr(path_start);
			}
			else {
				parts.authority = uri_without_scheme.substr(authority_start);
				parts.has_authority = true;
			}
		}		
		else {
			// only a path, no authority
			parts.path = uri_without_scheme;
		}
		return parts;
	}

	Url::Url(const std::string& url) :
		Url()
	{
		UriParts parts = parseUri(url);
		m_scheme = parts.scheme;
		m_hostname = parts.authority;
		m_path = parts.path;
		m_query = parts.query;
		m_fragment = parts.fragment;
		m_has_query = parts.has_query;
		m_has_fragment = parts.has_fragment;
		std::string port_text;

		m_is_empty = url.empty();

		if (!parts.has_authority || parts.authority.empty())
			return;

		// Userinfo is not supported in this implementation, so if the authority contains '@', we consider it invalid.
		if (parts.authority.find('@') != std::string::npos) {
			m_is_valid = false;
			return;
		}
		// ipv6 address handling: if the authority starts with '[', it should be an IPv6 address, and we need to find the closing ']'.
		if (parts.authority.front() == '[') {
			const auto close = parts.authority.find(']');
			if (close == std::string::npos) {
				m_is_valid = false;
				return;
			}

			m_hostname = parts.authority.substr(1, close - 1);

			// check hostname for a valid ipv6 address format (basic check)
			for (char c : m_hostname) {
				if (hexValue(static_cast<unsigned char>(c)) == -1 && c != ':' && c != '.') {
					m_is_valid = false;
					return;
				}
			}

			// After ] only :port is allowed.
			if (close + 1 < parts.authority.size()) {
				if (parts.authority[close + 1] != ':') {
					m_is_valid = false;
					return;
				}

				m_has_explicit_port = true;
				port_text = parts.authority.substr(close + 2);				
			}
		}
		else {
			// ipv4 or hostname, possibly with port
			size_t colon_pos = parts.authority.find(':');
			if (colon_pos != std::string::npos) {
				m_hostname = parts.authority.substr(0, colon_pos);

				// check hostname for valid characters (basic check)
				for (char c : m_hostname) {
					if (!isAlpha(static_cast<unsigned char>(c)) && !isDigit(static_cast<unsigned char>(c)) && c != '-' && c != '.') {
						m_is_valid = false;
						return;
					}
				}

				port_text = parts.authority.substr(colon_pos + 1);
				m_has_explicit_port = true;
			}
			else {
				m_hostname = parts.authority;
				m_port = 0; // default port will be determined by scheme
				m_has_explicit_port = false;
			}
		}		

		// m_scheme must be lowercase for comparison purposes
		std::transform(m_scheme.begin(), m_scheme.end(), m_scheme.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c));	});
		if (!parts.has_scheme ||
			(m_scheme != "http" && m_scheme != "https"))
			return;

		m_is_cryptographic = m_scheme == "https";

		unsigned short port = m_scheme == "https" ? 443 : 80;

		if (m_has_explicit_port) {
			if (port_text.empty()) {
				m_is_valid = false;
				return;
			}
			port = 0;
			for (char c : port_text) {
				if (c < '0' || c > '9') {
					m_is_valid = false;
					return;
				}

				const int digit = c - '0';
				if (port > (65535 - digit) / 10) {
					m_is_valid = false;
					return;
				}

				port = port * 10 + digit;
			}
		}
		m_port = port;

		if (parts.has_scheme && parts.has_authority && m_hostname.empty()) {
			// Invalid URL: authority is present but hostname is empty
			m_is_valid = false;
			return;
		}

		m_is_valid = true;
	}

	// full url as string
	std::string Url::toString() const {
		if (!m_is_valid)
			return "";
		return m_scheme + "://" + hostHeader()
			+ m_path
			+ (m_has_query ? "?" + m_query : "")
			+ (m_has_fragment ? "#" + m_fragment : "");
	}

	std::string Url::withoutFragment() const {
		if (!m_is_valid)
			return "";
		return m_scheme + "://" + hostHeader()
			+ m_path
			+ (m_has_query ? "?" + m_query : "");
	}

	bool Url::hasExplicitPort() const {
		if (!m_is_valid)
			return false;
		return m_has_explicit_port;
	}

	std::string Url::requestTarget() const {
		if (!m_is_valid)
			return "";
		return (m_path.empty() ? "/" : m_path) + (m_has_query ? "?" + m_query : "");
	}

	std::string Url::hostHeader() const {
		if (!m_is_valid)
			return "";
		const std::string host =
			m_hostname.find(':') != std::string::npos
			? "[" + m_hostname + "]"
			: m_hostname;

		return host + (m_has_explicit_port
			? ":" + std::to_string(m_port)
			: "");
	}

	bool Url::resolveReference(const std::string& reference, Url& result) const
	{
		if (!m_is_valid)
			return false;
		UriParts ref = parseUri(reference);

		// If the reference has a scheme, it's absolute and we can just return it as is.		

		if (ref.has_scheme && ref.scheme.length()) {
			Url ref_url(reference);
			if (!ref_url.isValid())
				return false;
			ref_url.m_path = removeDotSegments(ref_url.m_path);			
			result = ref_url;
			return true;
		}		

		// If the reference has an authority, we replace the authority and path.
		if (ref.has_authority) {
			Url target(m_scheme + ":" + reference);

			if (!target.isValid())
				return false;

			target.m_path = removeDotSegments(target.m_path);
			result = target;
			return true;
		}

		result = *this;

		// The fragment always comes from the reference.
		result.m_has_fragment = ref.has_fragment;
		result.m_fragment = ref.fragment;

		if (ref.path.empty()) {
			// If there is no new query, the existing query remains.
			if (ref.has_query) {
				result.m_has_query = true;
				result.m_query = ref.query;
			}

			return true;
		}

		// If there is a new path, the existing query is replaced,
		// even if the reference does not contain a query.
		result.m_has_query = ref.has_query;
		result.m_query = ref.query;

		if (ref.path.front() == '/') {
			result.m_path = removeDotSegments(ref.path);
			return true;
		}

		// attach relative path to the base directory
		const auto slash = m_path.find_last_of('/');

		const std::string base_directory =
			slash == std::string::npos
			? "/"
			: m_path.substr(0, slash + 1);

		result.m_path = removeDotSegments(base_directory + ref.path);
		return true;
	}
}