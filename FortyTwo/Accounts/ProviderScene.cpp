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

User* ProviderScene::getUser(std::string user_name)
{
	if (!m_user_list && !m_avatarlist_scene.empty()) {
		// find the user list
		m_user_list = m_app->getUnitTree()->find(m_avatarlist_scene);
	}
	if (m_user_list) {
		User* user_info = doCast<User>(m_user_list->getChild(user_name));
		return user_info;
	}
	return nullptr;
}
