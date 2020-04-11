#ifndef ORION_REALMS_H
#define ORION_REALMS_H

#include <vector>

using std::vector;

#include "Realm.h"
#include "Event.h"
#include "EventHandler.h"

namespace Orion
{
	class Orion;

	class RealmResolvedEvent : public Event
	{
	protected:
		Realm* m_realm;
	public:
		RealmResolvedEvent(Realm* realm) : m_realm(realm) {}
		Realm* getRealm() { return m_realm; }
	};

	class Realms
	{
	protected:
		Orion* m_orion;
		std::vector<Realm*> m_realms;
		std::vector<Realm*> m_file_realms;
		time_t m_realms_lastmod = 0;

	public:
		Realms(Orion* orion);
		// get realm from cached realm list
		Realm* getRealmCached(string name);
		// get realm asynchronously from server
		int getRealm(string name, EventHandler *handler);
		// add realm to realm list
		void addRealm(Realm* realm);
	};
}

#endif // !ORION_REALMS_H
