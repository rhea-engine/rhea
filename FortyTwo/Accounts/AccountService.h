/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef FORTYTWO_ACCOUNTSERVICE_H
#define FORTYTWO_ACCOUNTSERVICE_H

#include <memory>
#include <Rhea/Service.h>
#include "Provider.h"
#include <FortyTwo/Authorization/AuthInfo.h>

class User;

class AccountService : public rhea::Service
{
	MECLASS(AccountService, rhea::Service)
public:
	AccountService(std::string name, std::string type) :
		rhea::Service(name, type)
	{}

	void setup(rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);
	// set a provider directly
	void setProvider(std::unique_ptr<Provider> provider);

	// get provider to act on it
	Provider* getProvider() { return m_provider.get(); }

	void GetAccount(AuthInfo& auth_info, std::function<void(User*, int)> handler);

	void CreateAvatar(User* user, rhea::Avatar* avatar, std::function<void(int)> handler);

	// create a new account
	void CreateAccount(AuthInfo& auth_info, std::function<void(User*, int)> handler);

	// setup

	std::vector<rhea::Unit::Export> getReferences();

	static void RegisterFabrics(rhea::Application* app);

protected:
	std::unique_ptr<Provider> m_provider;
	std::string m_provider_type;
};

#endif // !FORTYTWO_ACCOUNTSERVICE_H
