#ifndef ORION_REALM_H
#define ORION_REALM_H

#include <string>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using std::string;

namespace Orion
{
	class Realm
	{
	protected:
		string m_name;
		string m_resource_url;
		string m_content_type;
		time_t m_ttl;

		friend void to_json(json& j, const Realm& e);
		friend void from_json(const json& j, Realm& e);
	public:
		Realm(): m_ttl(time(NULL)) {}		
		Realm(string name);
		Realm(string name, string resource_url, time_t ttl);
		Realm(Realm& r);
		void setResourceUrl(string url);
		void setTTL(time_t ttl);

		// return wheter ttl is expired
		bool isExpired();

		// get resource url
		string getResourceUrl() { return m_resource_url; }

		// get name of realm
		string getName() { return m_name; }
	};

	void to_json(json& j, const Realm& e);
	void from_json(const json& j, Realm& e);
}

#endif // !ORION_REALM_H
