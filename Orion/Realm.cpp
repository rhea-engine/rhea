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
}