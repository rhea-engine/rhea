#include <Orion/Realm.h>
#include <time.h>

#include <nlohmann/json.hpp>

namespace Orion
{
	Realm::Realm(std::string name)
		: m_name(std::move(name))
		, m_ttl(std::time(nullptr))
	{
	}

	Realm::Realm(std::string name, std::string resource_url, std::time_t ttl)
		: m_name(std::move(name))
		, m_resource_url(std::move(resource_url))
		, m_ttl(std::time(NULL) + ttl)
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