/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Orion/Realm.h>
#include <time.h>

#include <nlohmann/json.hpp>

namespace orion
{
	Realm::Realm(std::string name)
		: m_name(std::move(name))
		, m_ttl(std::time(nullptr)),
		m_realm_type(RealmType::URL)
	{
	}

	Realm::Realm(std::string name, std::string resource_url, std::time_t ttl)
		: m_name(std::move(name))
		, m_resource_url(std::move(resource_url))
		, m_ttl(std::time(NULL) + ttl),
		m_realm_type(RealmType::URL)
	{
	}

	Realm::Realm(std::string name, std::string path, RealmType type)
		: m_name(std::move(name))
		, m_resource_url(std::move(path))
		, m_ttl(0)
		, m_realm_type(type)
	{
	}

	void Realm::setResourceUrl(std::string url)
	{
		m_resource_url = std::move(url);
	}

	void Realm::setContentType(std::string contentType)
	{
		m_content_type = std::move(contentType);
	}

	void Realm::setTTL(std::time_t ttl)
	{
		m_ttl = ttl;
	}

	bool Realm::isExpired() const
	{		
		return (std::time(NULL) - m_ttl) < 0;
	}

	orion::Error Realm::resolveFilePath(const std::string& resource_path, std::string& native_path) const
	{
		native_path.clear();
		// ResourceRequest supplies a normalized, realm-rooted path.
		if (resource_path.empty() || resource_path.front() != '/' || resource_path.find('\\') != std::string::npos)
			return Error::DATA_INVALID;

		if (m_realm_type == RealmType::URL) {
			// For URL realms, we cannot resolve to a local file path
			return orion::Error(orion::Error::PATH_NOT_FOUND);
		} else if (m_realm_type == RealmType::LocalPath) {
			if (m_resource_url.empty())
				return Error::DATA_INVALID;

			native_path = m_resource_url;

			if (native_path.back() == '/' || native_path.back() == '\\')
				native_path += resource_path.substr(1);
			else
				native_path += resource_path;

			return Error::OK;
		}
		else if (m_realm_type == RealmType::FileSystem) {
#if defined(_WIN32)
			// The virtual drive overview has no native filesystem path.
			if (resource_path == "/")
				return Error::DATA_INVALID;

			if (resource_path.compare(0, 5, "/unc/") == 0) {
				const size_t server_begin = 5;
				const size_t server_end = resource_path.find('/', server_begin);

				if (server_end == std::string::npos || server_end == server_begin)
					return Error::DATA_INVALID;

				const size_t share_begin = server_end + 1;
				const size_t share_end = resource_path.find('/', share_begin);
				const size_t share_length = share_end == std::string::npos
					? resource_path.size() - share_begin
					: share_end - share_begin;

				if (share_length == 0)
					return Error::DATA_INVALID;

				native_path = "//" + resource_path.substr(server_begin);
				return Error::OK;
			}

			// Require /C:/... rather than the drive-relative form /C:...
			if (resource_path.size() < 4)
				return Error::DATA_INVALID;

			const char drive = resource_path[1];
			const bool valid_drive =
				(drive >= 'A' && drive <= 'Z') ||
				(drive >= 'a' && drive <= 'z');

			if (!valid_drive || resource_path[2] != ':' || resource_path[3] != '/')
				return Error::DATA_INVALID;

			native_path = resource_path.substr(1);
#else
			native_path = resource_path;
#endif

			return Error::OK;

		} else {
			// Unknown realm type
			return orion::Error(orion::Error::DATA_INVALID);
		}
		return orion::Error();
	}
}