/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Realm.h"

namespace rhea
{
	void Realm::setRealm(orion::Realm* realm)
	{
		m_realm = realm;
	}
}