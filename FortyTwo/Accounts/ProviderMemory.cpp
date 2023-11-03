#include "ProviderMemory.h"
#include "User.h"

User* ProviderMemory::getUser(std::string user_name)
{
	if (m_users.find(user_name) != m_users.end())
		return m_users[user_name];
	else
		return nullptr;
}

void ProviderMemory::addUser(User* user)
{
	m_users[user->getPathName()] = user;
}
