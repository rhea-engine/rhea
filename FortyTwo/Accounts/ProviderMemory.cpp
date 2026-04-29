/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "ProviderMemory.h"
#include "User.h"

void ProviderMemory::GetAccount(AuthInfo& auth_info, std::function<void(User*, int)> handler)
{
	if (m_users.find(auth_info.user_id) != m_users.end())
		handler(m_users[auth_info.user_id], 0);
	else
		handler(nullptr, -1);
}

void ProviderMemory::CreateAccount(std::string user_id, std::string name, std::function<void(User*, int)> handler)
{
	User* user = new User(user_id);
	user->setDisplayName(name);
	user->setCreationTime(time(0));
	user->setLastLoginTime(time(0));
	m_users[user_id] = user;

	handler(user, 0);
}

void ProviderMemory::CreateAvatar(User* user, rhea::Avatar* avatar, std::function<void(int)> handler)
{
	// check if name exists
	for (auto& u : m_users) {
		rhea::Avatar* check = doCast<rhea::Avatar>(u.second->find(avatar->getPathName()));
		if (check)
			return handler(1);
	}
	// all ok
	user->addChild(avatar);
	handler(0);
}
