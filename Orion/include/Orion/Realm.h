/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ORION_REALM_H
#define ORION_REALM_H

#include <string>
#include <ctime>

#include <Orion/RequestError.h>

namespace orion
{
	class Realm
	{
	public:
		enum class RealmType {
			URL,		// HTTP/HTTPS URLs
			LocalPath,   // local path mappings
			FileSystem,  // local file system access
		};

		Realm(): m_ttl(time(NULL)), m_realm_type(RealmType::URL) {}		
		Realm(std::string name);
		Realm(std::string name, std::string resource_url, time_t ttl);
		Realm(std::string name, std::string local_path, RealmType type);

		// default copy/assign is fine
		Realm(const Realm&) = default;
		Realm& operator=(const Realm&) = default;

		void setResourceUrl(std::string url);
		void setContentType(std::string contentType);
		void setTTL(std::time_t ttl);
		void setType(RealmType type) { m_realm_type = type; }

		// return wheter ttl is expired
		bool isExpired() const;

		// get resource url
		const std::string& getResourceUrl() const { return m_resource_url; }
		// get name of realm
		const std::string& getName() const { return m_name; }
		const std::string& getContentType() const { return m_content_type; }
		std::time_t getTTL() const { return m_ttl; }
		RealmType getType() const { return m_realm_type; }
		bool isLocalPath() const { return m_realm_type == RealmType::LocalPath; }

		// normalize a resource path to a native path
		Error resolveFilePath(const std::string& resource_path, std::string& native_path) const;

	private:
		std::string m_name;
		std::string m_resource_url;
		std::string m_content_type;
		std::time_t m_ttl{};
		RealmType m_realm_type;
	};
}

#endif // !ORION_REALM_H
