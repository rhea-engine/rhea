/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef FORTYTWO_ACCOUNTS_PROVIDER_SCENE_H
#define FORTYTWO_ACCOUNTS_PROVIDER_SCENE_H

#include <FortyTwo/Accounts/Provider.h>

class ProviderScene : public Provider
{
public:
	ProviderScene() :
		m_app(nullptr),
		m_user_list(nullptr)
	{}

	// setup phase
	void Setup(rhea::Application* app, std::unordered_map<std::string, Variant>& values);
	
	std::vector<rhea::Unit::ResourceReference> getReferences();

	virtual void GetAccount(AuthInfo& auth_info, std::function<void(User*, int)> handler) override;

	virtual void CreateAccount(std::string user_id, std::string name, std::function<void(User*, int)> handler) override; 

	virtual void CreateAvatar(User* user, rhea::Avatar* avatar, std::function<void(int)> handler) override;

protected:
	rhea::Application* m_app;
	std::string m_avatarlist_scene;
	rhea::Unit* m_user_list;

	// store the user list
	void Save(std::function<void(int)> handler);
};

#endif // !FORTYTWO_ACCOUNTS_PROVIDER_SCENE_H
