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
	void Setup(Rhea::Application* app, std::unordered_map<std::string, Variant>& values);
	
	std::vector<Rhea::Unit::Export> getReferences();

	virtual void GetAccount(AuthInfo& auth_info, std::function<void(User*, int)> handler) override;

	virtual void CreateAccount(std::string user_id, std::string name, std::function<void(User*, int)> handler) override; 
	
protected:
	Rhea::Application* m_app;
	std::string m_avatarlist_scene;
	Rhea::Unit* m_user_list;
};

#endif // !FORTYTWO_ACCOUNTS_PROVIDER_SCENE_H
