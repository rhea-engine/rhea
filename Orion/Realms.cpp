/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Orion/Realms.h>
#include <Orion/Orion.h>
#include <Orion/FileStream.h>
#include <Orion/ResourceStream.h>
#include <Orion/RealmRequest.h>
#include <algorithm>

namespace orion
{
	Realms::Realms(Orion* orion) : m_orion(orion)
	{
		LoadIrisServers();
	}

	Realm* Realms::getRealmCached(std::string name)
	{
		std::vector<Realm*>::iterator it;

		// check if realm is installed locally
		if (m_orion->checkResourceFromCache(name, "")) {
			Realm* r = new Realm(name);
			m_realms.push_back(r);
			return r;
		}

		// Only load file_realms once on the first call
		if (!m_file_realms_loaded) {
			LoadFileRealms();
			m_file_realms_loaded = true;
		}

		// get from file realm list
		for (auto it = m_file_realms.begin(); it != m_file_realms.end(); ++it)
		{
			Realm* r = *it;
			if (!name.compare(r->getName())) {
				return r;
			}
		}

		// get from server realm list
		for (auto it = m_realms.begin(); it != m_realms.end(); ++it)
		{
			Realm* r = *it;
			if (r->isExpired()) {
				delete r;
				m_realms.erase(it);
			}
			else
				if (!name.compare(r->getName()))
					return r;
		}
		// not found in cache
		return NULL;
	}

	void Realms::LoadFileRealms()
	{
		std::string path = m_orion->getFs()->getInstallDataDir();
		path += std::string("/Meworla/Core/realms.conf");
		
		if (!psio::Fs::fileExists(path))
			return;

		FileResourceStream* res = new FileResourceStream(path);
		res->Open();
		m_realms_lastmod = res->getLastmod();

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
			size_t pos = s.find_first_of(" \t");
			if (pos != std::string::npos) {
				std::string realmname = s.substr(0, pos);
				std::string url = s.substr(pos + 1, std::string::npos);
				
				Realm* realm = nullptr;
				if (url.find("file://") == 0) {
					// Entferne file:// Prefix
					std::string localPath = url.substr(7);
					realm = new Realm(realmname, localPath, Realm::RealmType::LocalPath);
				}
				else if (url.length() > 0 && (url[0] == '/' || url[0] == '\\' || 
						(url.length() > 2 && url[1] == ':'))) {
					// Absoluter Pfad
					realm = new Realm(realmname, url, Realm::RealmType::LocalPath);
				}
				else {
					// Standard URL
					realm = new Realm(realmname, url, Realm::RealmType::URL);
				}
				
				m_file_realms.push_back(realm);
			}
		}
		delete res;
	}

	Error Realms::getRealm(std::string name, ResolveHandler handler)
	{
		// no check of realms list - we assume that was done before by the client
		
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
				if (error == orion::Error::OK) {
					addRealm(realm);
				}

				// call all handlers
				for (auto it = m_requests[name].handlers.begin(); it != m_requests[name].handlers.end(); ++it) {
					ResolveHandler result_handler = *it;
					result_handler(realm, error);
				}

				RealmRequest* req = m_requests[name].request;
				m_requests[name].request = nullptr;
				m_requests.erase(name);
				delete req;
				});			
		}

		// data is pending
		return Error::IO_PENDING;
	}

	void Realms::addRealm(Realm* realm)
	{
		m_realms.push_back(realm);
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

	void Realms::reloadFileRealms()
	{
	    std::string path = m_orion->getFs()->getInstallDataDir();
	    path += std::string("/Meworla/Core/realms.conf");
	    
	    if (!psio::Fs::fileExists(path))
	        return;

	    FileResourceStream* res = new FileResourceStream(path);
	    res->Open();
	    
	    // Only reload if the file has changed
	    if (m_realms_lastmod >= res->getLastmod()) {
	        delete res;
	        return;
	    }
	    
	    m_realms_lastmod = res->getLastmod();
	    
	    // Clear old file realms
	    for (auto it = m_file_realms.begin(); it != m_file_realms.end(); ++it)
	        delete *it;
	    m_file_realms.clear();
	    
	    // Re-parse...
	    delete res;
		LoadFileRealms();
	}
}