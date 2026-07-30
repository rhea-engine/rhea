/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef FORTYTWO_ACCOUNTS_PROVIDER_H
#define FORTYTWO_ACCOUNTS_PROVIDER_H

#include <Rhea/Application.h>
#include <FortyTwo/Authorization/AuthInfo.h>
#include <Rhea/Core/Avatar.h>

class User;

class Provider
{
public:
	virtual ~Provider() {}

	virtual void Setup(rhea::Application* app, std::unordered_map<std::string, Variant> &values) = 0;

	virtual std::vector<rhea::Unit::> getReferences() = 0;

	// get the account with the given user name
	// if user account not exists create one
	virtual void GetAccount(AuthInfo &auth_info, std::function<void(User*, int)> handler) = 0;

	// add a user to the storage
	virtual void CreateAccount(std::string user_id, std::string name, std::function<void(User*, int)> handler) = 0;

	// add an avatar to the user account and store ist
	virtual void CreateAvatar(User* user, rhea::Avatar* avatar, std::function<void(int)> handler) = 0;
};

#endif // !FORTYTWO_ACCOUNTS_PROVIDER_H
