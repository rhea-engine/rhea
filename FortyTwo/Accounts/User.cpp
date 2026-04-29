/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "User.h"

rhea::Avatar* User::getAvatar(std::string name)
{
	rhea::Avatar* avatar = doCast<rhea::Avatar>(getChild(name));
	return avatar;
}

void User::Avatars(std::function<bool(rhea::Avatar*)> handler)
{
	children([handler](std::string& name, rhea::Component* unit)->bool {
		rhea::Avatar* avatar = doCast<rhea::Avatar>(unit);
		if (avatar)
			return handler(avatar);
		else
			return true;
		});
}

void User::ready()
{
	if (m_values["name"] != Variant())
		m_display_name = m_values["name"].toString();
}
