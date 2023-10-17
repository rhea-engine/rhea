#include "Realms.h"
#include "Orion.h"
#include "FileStream.h"
#include "ResourceStream.h"
#include "RealmRequest.h"

namespace Orion
{
	Realms::Realms(Orion* orion) : m_orion(orion)
	{
		LoadIrisServers();
	}

	Realm* Realms::getRealmCached(string name)
	{
		std::vector<Realm*>::iterator it;

		// get from realms.conf
		string path = m_orion->getFs()->getInstallDataDir();
		path += string("/Meworla/Core/realms.conf");
		if (Psio::Fs::fileExists(path))
		{
			FileResourceStream* res = new FileResourceStream(path);
			res->Open();
			if (m_realms_lastmod < res->getLastmod())
			{
				m_realms_lastmod = res->getLastmod();
				for (auto it = m_realms.begin(); it != m_realms.end(); ++it)
					delete* it;
				m_file_realms.clear();

				while (!res->eof()) {
					string s = res->getLine();

					// remove leading whitespace
					s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
						return !std::isspace(ch);
						}));

					// skip empty and comment lines
					if (s.length() == 0 || (s.at(0) == '#'))
						continue;

					// parse the line
					size_t pos = s.find_first_of(" \t");
					if (pos != string::npos) {
						// parse successful, store it to file realms list
						string realmname = s.substr(0, pos);
						string url = s.substr(pos + 1, string::npos);
						Realm* realm = new Realm(realmname, url, 0);						
						m_file_realms.push_back(realm);
					}
				}
			}
			delete res;
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

	int Realms::getRealm(string name, ResolveHandler handler)
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
			req->Lookup(name, [&, name](Realm *realm, int error) {
				// store the realm
				if (!error) {
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
		return -1;
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
		if (Psio::Fs::fileExists(path))
		{
			FileResourceStream* res = new FileResourceStream(path);
			res->SetMode(Psio::Fs::READ);
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
}