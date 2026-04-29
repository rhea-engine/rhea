/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef FORTYTWO_PROVIDER_MEMORY_H
#define FORTYTWO_PROVIDER_MEMORY_H

#include "Provider.h"

#include <unordered_map>

class ProviderMemory : public Provider
{
public:
	void Setup(rhea::Application* app, std::unordered_map<std::string, Variant>& values) {}

	std::vector<rhea::Unit::Export> getReferences() {
		return std::vector<rhea::Unit::Export>();
	}

	virtual void GetAccount(AuthInfo& auth_info, std::function<void(User*, int)> handler) override;

	virtual void CreateAccount(std::string user_id, std::string name, std::function<void(User*, int)> handler) override;

	virtual void CreateAvatar(User* user, rhea::Avatar* avatar, std::function<void(int)> handler) override;


protected:



	std::unordered_map<std::string, User*> m_users;
};

#endif // !FORTYTWO_PROVIDER_MEMORY_H
