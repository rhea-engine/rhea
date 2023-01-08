#include "Realm.h"
#include <time.h>

namespace Orion
{
	Realm::Realm(string name)
	{
		m_name = name;
		m_resource_url = "";
		m_ttl = 0;
	}

	Realm::Realm(string name, string resource_url, time_t ttl)
	{
		m_name = name;
		m_resource_url = resource_url;
		m_ttl = time(NULL) + ttl;
	}

	Realm::Realm(Realm& r)
	{
		m_name = r.m_name;
		m_resource_url = r.m_resource_url;
		m_content_type = r.m_content_type;
		m_ttl = r.m_ttl;
	}

	void Realm::setResourceUrl(string resource_url)
	{
		m_resource_url = resource_url;
	}

	void Realm::setTTL(time_t ttl)
	{
		m_ttl = time(NULL) + ttl;
	}

	bool Realm::isExpired()
	{		
		return (time(NULL) - m_ttl) < 0;
	}

	void to_json(json& j, const Realm& e) {
		j = json{ {"name", e.m_name}, {"type", e.m_content_type}, {"href", e.m_resource_url}, {"ttl", e.m_ttl}};
	}

	void from_json(const json& j, Realm& e) {
		j.at("name").get_to(e.m_name);
		j.at("type").get_to(e.m_content_type);
		j.at("href").get_to(e.m_resource_url);
		j.at("ttl").get_to(e.m_ttl);
		e.m_ttl += time(NULL);
	}	
}