#ifndef ORION_REALMS_H
#define ORION_REALMS_H

#include <vector>
#include <deque>
#include <unordered_map>
#include <functional>
#include <ctime>
#include <string>

#include <Orion/Realm.h>
#include <Orion/Event.h>

namespace Orion
{
	class Orion;
	class RealmRequest;

	class Realms
	{
	public:

		typedef std::function<void(Realm*, int err)> ResolveHandler;

		Realms(Orion* orion);
		// get realm from cached realm list
		Realm* getRealmCached(std::string name);
		// get realm asynchronously from server
		int getRealm(std::string name, ResolveHandler handler);
		// add realm to realm list
		void addRealm(Realm* realm);

	protected:
		Orion* m_orion;
		std::vector<Realm*> m_realms;
		std::vector<Realm*> m_file_realms;
		std::time_t m_realms_lastmod = 0;

		void LoadIrisServers();

		struct resolving {
			RealmRequest* request;
			std::vector<ResolveHandler> handlers;
		};

		std::unordered_map<std::string, resolving> m_requests;
		std::deque<std::string> m_iris_services;
	};
}

#endif // !ORION_REALMS_H
