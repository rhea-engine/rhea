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
		MECLASS(Realm, Unit)
	public:
		Realm(Orion::Realm *realm, std::string pathname) :
			m_realm(realm),
			Unit(pathname, "//meworla/realm")
		{}
		Realm(std::string name, std::string type_name) :
			m_realm(NULL),
			Unit(name, type_name)
		{}
		Realm(std::string name) :
			Realm(name, "//meworla/realm")
		{}

		// set the realm
		void setRealm(Orion::Realm* realm);

	protected:
		Orion::Realm* m_realm;
	};
}

#endif // !RHEA_REALM_H
