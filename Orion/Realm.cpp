/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Orion/Realm.h>
#include <time.h>
#include <Orion/FileAddress.h>

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
		, m_ttl(std::time(NULL) + ttl),
		m_realm_type(RealmType::URL)
	{
		setResourceUrl(std::move(resource_url));
	}

	Realm::Realm(std::string name, std::string path, RealmType type)
		: m_name(std::move(name))
		, m_ttl(0)
		, m_realm_type(type)
	{
		setResourceUrl(std::move(path));
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
		if (m_realm_type == RealmType::LocalPath || m_realm_type == RealmType::FileSystem)
			return false;
		return (std::time(NULL) - m_ttl) >= 0;
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

			return resolveNativeResourcePath(m_resource_url, resource_path, native_path);
		} else {
			// Unknown or unsupported/not applicable realm type
			return orion::Error(orion::Error::DATA_INVALID);
		}
	}
}