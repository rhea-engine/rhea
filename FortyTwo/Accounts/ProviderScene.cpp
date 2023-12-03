#include "ProviderScene.h"
#include "User.h"
#include <Psio/Log.h>
#include <Rhea/UnitTree.h>

void ProviderScene::Setup(Rhea::Application* app, std::unordered_map<std::string, Variant>& values)
{
	m_app = app;

	if (values.find("avatarlist") != values.end()) {
		m_avatarlist_scene = (std::string)values["avatarlist"];
	}
	else {
		LOG(LOG_ERROR) << "ProviderScene: no avatarlist defined in AccountService configuration.";
	}
}

std::vector<Rhea::Unit::Export> ProviderScene::getReferences()
{
	std::vector<Rhea::Unit::Export> refs;
	if (!m_avatarlist_scene.empty())
		refs.push_back({ "avatarlist", "//meworla/avatarlist", m_avatarlist_scene });

	return refs;
}

void ProviderScene::GetAccount(AuthInfo& auth_info, std::function<void(User*, int)> handler)
{
	if (!m_user_list && !m_avatarlist_scene.empty()) {
		// find the user list
		m_user_list = m_app->getUnitTree()->find(m_avatarlist_scene);
	}
	if (m_user_list) {
		User* user_info = doCast<User>(m_user_list->getChild(auth_info.user_id));
		if (user_info) {
			handler(user_info, 0);
			return;
		}
	}
	handler(nullptr, -1);
}

void ProviderScene::CreateAccount(std::string user_id, std::string name, std::function<void(User*, int)> handler)
{
	// add user to user list
	// this does not add the user to the storage yet
	if (!m_user_list && !m_avatarlist_scene.empty()) {
		// find the user list
		m_user_list = m_app->getUnitTree()->find(m_avatarlist_scene);
	}
	if (m_user_list) {
		User* user = new User(user_id);
		user->setDisplayName(name);
		user->setCreationTime(time(0));
		user->setLastLoginTime(time(0));
		m_user_list->addChild(user);

		// TODO save the current state

		handler(user, 0);
		return;
	}
	handler(nullptr, -1);
}
