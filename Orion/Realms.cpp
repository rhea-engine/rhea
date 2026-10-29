/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Orion/Realms.h>
#include <Orion/Orion.h>
#include <Orion/FileStream.h>
#include <Orion/RealmRequest.h>
#include <algorithm>
#include <cctype>

namespace orion
{
	Realms::Realms(Orion* orion) : m_orion(orion)
	{
		LoadIrisServers();
	}

	RealmConfigResult Realms::loadConfigFiles(const std::vector<RealmConfigFile>& files)
	{
		RealmConfigResult result{};
		result.success = true;

		// Build a replacement configuration without changing the active one.
		std::unordered_map<std::string, Realm> next_realms;

		for (const RealmConfigFile& file : files) {
			RealmConfigDiagnostic diag = LoadFileRealms(file.path, next_realms);

			if (diag.error == RealmConfigError::None)
				continue;

			if (diag.error == RealmConfigError::NotFound && file.optional)
				continue;

			result.success = false;
			result.diagnostics.push_back(std::move(diag));
		}
		if (result.success) {
			m_config_realms.swap(next_realms);
			m_config_files = files;
		}

		return result;
	}


	Error Realms::getRealmCached(std::string name, Realm& realm)
	{
		if (name == "file") {
			if (m_resolved_realms.find(name) != m_resolved_realms.end() && m_resolved_realms[name].getType() == Realm::RealmType::FileSystem) {
				realm = m_resolved_realms[name];
				return Error::OK;

			}
			return Error::PATH_NOT_FOUND;
		}

		// check if realm is installed locally
		if (m_orion->checkResourceFromCache(name, "")) {
			m_resolved_realms[name] = Realm(name);
			realm = m_resolved_realms[name];
			return Error::OK;
		}

		// get from file realm list
		auto it = m_config_realms.find(name);
		if (it != m_config_realms.end()) {
			realm = it->second;
			return Error::OK;
		}

		// get from server realm list
		if (m_resolved_realms.find(name) != m_resolved_realms.end()) {

			Realm* cached_realm = &m_resolved_realms[name];
			if (!name.compare(cached_realm->getName())) {
				if (cached_realm->isExpired()) {
					m_resolved_realms.erase(name);
					return Error::PATH_NOT_FOUND;
				}
				else {
					realm = *cached_realm;
					return Error::OK;
				}
			}
		}
		// not found in cache
		return Error::PATH_NOT_FOUND;
	}

	RealmConfigDiagnostic Realms::LoadFileRealms(const std::string& path, std::unordered_map<std::string, Realm>& target)
	{
		RealmConfigDiagnostic diag{};
		
		if (!psio::Fs::fileExists(path)) {
			diag.error = RealmConfigError::NotFound;
			diag.message = "File not found";
			return diag;
		}

		FileResourceStream* res = new FileResourceStream(path);
		res->SetMode(psio::Fs::READ);
		res->Open();
		int line_number = 0;

		while (!res->eof()) {
			std::string s = res->getLine();
			line_number++;

			// remove leading whitespace
			s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
				return !std::isspace(ch);
			}));

			// skip empty and comment lines
			if (s.length() == 0 || (s.at(0) == '#'))
				continue;

			// parse the line
			size_t pos = s.find_first_of(" \t");
			if (pos != std::string::npos) {
				std::string realmname = s.substr(0, pos);
				std::string url = s.substr(pos + 1, std::string::npos);
				
				if (url.find("file://") == 0) {
					// Entferne file:// Prefix
					std::string localPath = url.substr(7);
					target[realmname] = Realm(realmname, localPath, Realm::RealmType::LocalPath);
				}
				else if (url.length() > 0 && (url[0] == '/' || url[0] == '\\' || 
						(url.length() > 2 && url[1] == ':'))) {
					// Absoluter Pfad
					target[realmname] = Realm(realmname, url, Realm::RealmType::LocalPath);
				}
				else {
					// Standard URL
					target[realmname] = Realm(realmname, url, Realm::RealmType::URL);
				}
			}
		}
		delete res;

		return diag;
	}

	Error Realms::getRealm(std::string name, ResolveHandler handler)
	{
		// alredy a request for this realm name pending?
		auto it = m_requests.find(name);
		if (it != m_requests.end()) {
			it->second.handlers.push_back(handler);
		}
		else {
			// no - create new request
			RealmRequest* req = new RealmRequest(m_orion, m_iris_services);
			m_requests[name].request = req;
			m_requests[name].handlers.push_back(handler);
			
			// start the request
			req->Lookup(name, [&, name](Realm *realm, orion::Error error) {
				// store the realm
				if (error == orion::Error::OK && realm != nullptr) {
					auto [it, inserted] = m_resolved_realms.insert_or_assign(name, *realm);

					delete realm;
					realm = &it->second;
				}

				// call all handlers
				auto it = m_requests.find(name);
				if (it == m_requests.end())
					return;

				Realm result;
				if (error == Error::OK) {
					if (realm)
						result = *realm;
					else
						error = Error::DATA_INVALID;
				}

				auto handlers = std::move(it->second.handlers);
				RealmRequest* req = it->second.request;
				m_requests.erase(name);
				for (auto& handler : handlers) {
					if (handler)
						handler(result, error);
				}
				delete req;
				});			
		}

		// data is pending
		return Error::IO_PENDING;
	}

	void Realms::addRealm(const Realm& realm)
	{
		m_resolved_realms[realm.getName()] = realm;
	}
	
	void Realms::LoadIrisServers()
	{
		// get lookup info from iris.conf
		std::string path = m_orion->getFs()->getInstallDataDir();
		path += std::string("/Meworla/iris.conf");
		if (psio::Fs::fileExists(path))
		{
			FileResourceStream* res = new FileResourceStream(path);
			res->SetMode(psio::Fs::READ);
			res->Open();
			while (!res->eof()) {
				std::string s = res->getLine();

				// remove leading whitespace
				s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
					return !std::isspace(ch);
					}));

				// skip empty and comment lines
				if (s.length() == 0 || (s.at(0) == '#'))
					continue;

				// parse the line
				m_iris_services.push_back(s);
			}
		}
	}

	RealmConfigResult Realms::reloadConfigFiles()
	{
		const auto files = m_config_files;
		return loadConfigFiles(files);
	}
}
