#ifndef RHEA_REALM_H
#define RHEA_REALM_H

#include "Unit.h"
#include "Resource.h"

namespace Orion
{
	class Realm;
}

namespace Rhea
{
	class Realm : public Resource
	{
	public:
		Realm(Orion::Realm *realm, std::string pathname) :
			m_realm(realm),
			Resource(pathname, "//meworla/realm")
		{
			m_status = Status::LOADED;
		}
		Realm(std::string name) :
			m_realm(NULL),
			Resource(name, "//meworla/realm")
		{}

		// set the realm
		void setRealm(Orion::Realm* realm);

		// load the realm from remote
		void load(ReadyHandler handler);

		// factory function
		static Unit* createType(std::string name, std::string type);
	protected:
		Orion::Realm* m_realm;
	};
}

#endif // !RHEA_REALM_H
