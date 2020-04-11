#ifndef ORION_REALM_H
#define ORION_REALM_H

#include <string>

using std::string;

namespace Orion
{
	class Realm
	{
	protected:
		string m_name;
		string m_resource_url;
		time_t m_ttl;
	public:
		Realm(string name);
		Realm(string name, string resource_url, time_t ttl);
		void setResourceUrl(string url);
		void setTTL(time_t ttl);

		// return wheter ttl is expired
		bool isExpired();

		// get resource url
		string getResourceUrl() { return m_resource_url; }

		// get name of realm
		string getName() { return m_name; }
	};
}

#endif // !ORION_REALM_H
