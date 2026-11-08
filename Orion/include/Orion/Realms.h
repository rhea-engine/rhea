/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ORION_REALMS_H
#define ORION_REALMS_H

#include <vector>
#include <deque>
#include <unordered_map>
#include <functional>
#include <ctime>
#include <string>
#include <cstdint>

#include <Orion/Realm.h>
#include <Orion/Event.h>
#include <Orion/RequestError.h>

namespace orion
{
	class Orion;
	class RealmRequest;

	struct RealmConfigFile {
		std::string path;
		bool optional = true;
	};

	enum class RealmConfigError {
		None,
		NotFound,
		ReadError,
		InvalidFormat
	};

	struct RealmConfigDiagnostic {
		RealmConfigError error = RealmConfigError::None;
		std::string path;
		size_t line = 0;
		std::string message;
	};

	struct RealmConfigResult {
		bool success = false;
		std::vector<RealmConfigDiagnostic> diagnostics;
	};

	class Realms
	{
	public:

		typedef std::function<void(Realm, orion::Error err)> ResolveHandler;

		Realms(Orion* orion);
		
		// Files ordered from lowest to highest priority.
		// Later files override earlier definitions of the same realm.
		RealmConfigResult loadConfigFiles(const std::vector<RealmConfigFile>& files);

		// Reload the last successfully configured set of files.
		RealmConfigResult reloadConfigFiles();	
		
		// get realm from cached realm list
		Error getRealmCached(std::string name, Realm& realm);

		// get realm asynchronously from server
		Error getRealm(std::string name, uint64_t& generation, ResolveHandler handler);

		// cancel a pending resolve request
		void cancelResolve(std::string name, uint64_t generation);

		// add realm to realm list
		void addRealm(const Realm& realm);

		// add project realm to realm list
		void addProjectRealm(const Realm& realm);

		// get realm by physical path
		Realm* getRealmByPath(const std::string& physical_directory);

		// load resolver config from the file path, which contains a list of base urls to query for realm resolution
		bool loadResolverConfig(const std::string& path);

	protected:
		Orion* m_orion;

		struct ResolveHandlerDesc {
			ResolveHandler handler;
			uint64_t generation;
		};
		
		RealmConfigDiagnostic loadFileRealms(const std::string& path, std::unordered_map<std::string, Realm>& target);

		struct resolving {
			RealmRequest* request;
			std::vector<ResolveHandlerDesc> handlers;
		};

		std::unordered_map<std::string, resolving> m_requests;
		std::deque<std::string> m_resolve_paths;

		// Permanent mappings read from configuration files.
		std::unordered_map<std::string, Realm> m_config_realms;

		// Expiring results returned by the resolver service.
		std::unordered_map<std::string, Realm> m_resolved_realms;

		// Project realms that are not persisted to configuration files.
		std::unordered_map<std::string, Realm> m_project_realms;

		std::vector<RealmConfigFile> m_config_files;

		// request generation counter
		uint64_t m_request_generation = 0;
	};
}

#endif // !ORION_REALMS_H
