#ifndef ORION_REALM_H
#define ORION_REALM_H

#include <string>
#include <ctime>

namespace Orion
{
	class Realm
	{
	public:
		Realm(): m_ttl(time(NULL)) {}		
		Realm(std::string name);
		Realm(std::string name, std::string resource_url, time_t ttl);

		// default copy/assign is fine
		Realm(const Realm&) = default;
		Realm& operator=(const Realm&) = default;

		void setResourceUrl(std::string url);
		void setContentType(std::string contentType);
		void setTTL(std::time_t ttl);

		// return wheter ttl is expired
		bool isExpired() const;

		// get resource url
		const std::string& getResourceUrl() const { return m_resource_url; }
		// get name of realm
		const std::string& getName() const { return m_name; }
		const std::string& getContentType() const { return m_content_type; }
		std::time_t getTTL() const { return m_ttl; }


	private:
		std::string m_name;
		std::string m_resource_url;
		std::string m_content_type;
		std::time_t m_ttl{};
	};
}

#endif // !ORION_REALM_H
