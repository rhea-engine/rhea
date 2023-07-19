#ifndef RHEA_REALM_H
#define RHEA_REALM_H

#include "Unit.h"

namespace Orion
{
	class Realm;
}

namespace Rhea
{
	class Realm : public Unit
	{
	public:
		Realm(Orion::Realm *realm, std::string pathname) :
			m_realm(realm),
			Unit(pathname, "//meworla/realm")
		{}

		Realm(std::string name) :
			m_realm(NULL),
			Unit(name, "//meworla/realm")
		{}

		// set the realm
		void setRealm(Orion::Realm* realm);

		// factory function
		static Unit* createType(std::string name, std::string type);
	protected:
		Orion::Realm* m_realm;
	};
}

#endif // !RHEA_REALM_H
