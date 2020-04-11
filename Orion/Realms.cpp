#include "Realms.h"
#include "Orion.h"
#include "FileResource.h"
#include "ResourceStream.h"

namespace Orion
{
	Realms::Realms(Orion* orion) : m_orion(orion)
	{
	}

	Realm* Realms::getRealmCached(string name)
	{
		std::vector<Realm*>::iterator it;

		// get from realms.conf
		string path = m_orion->getFs()->getInstallDataDir();
		path += string("/Meworla/Core/realms.conf");
		if (Fs::fileExists(path))
		{
			FileResource* res = new FileResource(path);
			if (m_realms_lastmod < res->getLastmod())
			{
				ResourceStream* rs = res->Open(1);
				m_realms_lastmod = res->getLastmod();
				for (auto it = m_realms.begin(); it != m_realms.end(); ++it)
					delete* it;
				m_file_realms.clear();

				while (!rs->eof()) {
					string s = rs->getLine();

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
				delete rs;
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

	int Realms::getRealm(string name, EventHandler* handler)
	{
		// data is pending
		return -1;
	}

	void Realms::addRealm(Realm* realm)
	{
		m_realms.push_back(realm);
	}
}