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
}