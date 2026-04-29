/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef FORTYTWO_AUTHBACKENDFILE_H
#define FORTYTWO_AUTHBACKENDFILE_H

#include "AuthBackend.h"

class AuthBackendFile : public AuthBackend
{
public:
	virtual void setup(rhea::Application* app) override;
	virtual void LookupByUserID(std::string user_id, DBLookupHandler handler) override;
	virtual void LookupByUsername(std::string user_id, DBLookupHandler handler) override;
	virtual void LookupByExternalID(std::string ext_id, DBLookupHandler handler) override;
	virtual void LookupBySessionKey(std::string session_id, DBLookupHandler handler) override;
	virtual void SetSessionKey(std::string user_id, std::string session_id, std::function<void(int err)> handler) override;

	virtual void CreateUser(UserInfo info, std::vector<std::string> secrets, std::function<void(std::string, int err)> handler) override;

	class UserStore {
	public:
		UserStore() {};
		AuthBackendFile::UserStore(const UserStore& e);

		// return UserPrefs as json
		virtual std::string toJSON();

		int	getNofUsers() { return (int)m_users.size(); }
		UserInfo getUser(int i) { return m_users[i]; }
		std::vector<UserInfo> getUsers() { return m_users; }

		std::vector<UserInfo> m_users;
	};

protected:

	rhea::Application* m_app = nullptr;
	UserStore m_store;

	void loadUsers();
	void saveUsers();
};

#endif // !FORTYTWO_AUTHBACKENDFILE_H
