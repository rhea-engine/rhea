/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_REALM_H
#define RHEA_REALM_H

#include "Unit.h"

namespace orion
{
	class Realm;
}

namespace rhea
{
	class Realm : public Unit
	{
		MECLASS(Realm, Unit)
	public:
		Realm(orion::Realm *realm, std::string pathname) :
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
		void setRealm(orion::Realm* realm);

	protected:
		orion::Realm* m_realm;
	};
}

#endif // !RHEA_REALM_H
