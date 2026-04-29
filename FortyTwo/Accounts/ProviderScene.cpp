/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "ProviderScene.h"
#include "User.h"
#include <Psio/Log.h>
#include <Rhea/UnitTree.h>
#include <Orion/FileStream.h>

void ProviderScene::Setup(rhea::Application* app, std::unordered_map<std::string, Variant>& values)
{
	m_app = app;

	if (values.find("avatarlist") != values.end()) {
		m_avatarlist_scene = (std::string)values["avatarlist"];
	}
	else {
		LOG(LOG_ERROR) << "ProviderScene: no avatarlist defined in AccountService configuration.";
	}
}

std::vector<rhea::Unit::Export> ProviderScene::getReferences()
{
	std::vector<rhea::Unit::Export> refs;
	if (!m_avatarlist_scene.empty())
		refs.push_back({ "avatarlist", "//meworla/avatarlist", m_avatarlist_scene });

	return refs;
}

void ProviderScene::GetAccount(AuthInfo& auth_info, std::function<void(User*, int)> handler)
{
	if (!m_user_list && !m_avatarlist_scene.empty()) {
		// find the user list
		m_user_list = doCast<rhea::Unit>(m_app->getUnitTree()->find(m_avatarlist_scene));
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
		m_user_list = doCast<rhea::Unit>(m_app->getUnitTree()->find(m_avatarlist_scene));
	}
	if (m_user_list) {
		User* user = new User(user_id);
		user->setDisplayName(name);
		user->setCreationTime(time(0));
		user->setLastLoginTime(time(0));
		m_user_list->addChild(user);

		// TODO save the current state
		Save([&, handler, user](int err) {
			if (!err)
				handler(user, 0);
			else
				handler(nullptr, -1);
			});
		return;
	}
	handler(nullptr, -1);
}

void ProviderScene::CreateAvatar(User* user, rhea::Avatar* avatar, std::function<void(int)> handler)
{
	// check for uniqueness
	if (!m_user_list && !m_avatarlist_scene.empty()) {
		// find the user list
		m_user_list = doCast<rhea::Unit>(m_app->getUnitTree()->find(m_avatarlist_scene));
		if (!m_user_list) {
			return handler(2);
		}
	}
	rhea::Avatar* check = doCast<rhea::Avatar>(m_user_list->find(avatar->getPathName()));
	if (check)
		return handler(1);
	// does not exist yet
	user->addChild(avatar);
	Save(handler);
}

void ProviderScene::Save(std::function<void(int)> handler)
{
	orion::Orion* fs = m_app->getFS();

	orion::ResourceRequest* resource = fs->getResourceByRid(m_avatarlist_scene);
	resource->resolve([&, handler](orion::ResourceRequest* req, int err) {
		if (err) {
			LOG(LOG_ERROR) << "ProviderScene: failed to resolve avatarlist scene";	
			handler(2);
		}
		else {
			psio::IOBuffer* buf = new psio::IOBuffer(64 * 1024 * 1024);
			orion::FileResourceStream* stream = (orion::FileResourceStream*)req->getStream();
			stream->SetMode(psio::Fs::WRITE);		

			m_user_list->children([&, buf](std::string name, rhea::Component* unit)->bool {
				User* user = doCast<User>(unit);
				if (user) {
					int size = snprintf(buf->writer(), buf->FreeBufSize(), "[%s]\ntype=\"//meworla/user\"\nname=\"%s\"\n\n",
						name.c_str(), user->getDisplayName().c_str());
					buf->Produce(size);
					user->children([&, name, buf](std::string avatar_name, rhea::Component* unit)->bool {
						rhea::Avatar* avatar = doCast<rhea::Avatar>(unit);
						if (avatar) {
							int size = snprintf(buf->writer(), buf->FreeBufSize(), "[%s/%s]\ntype=\"//meworla/avatar\"\nname=\"%s\"\n",
								name.c_str(), avatar_name.c_str(), avatar_name.c_str());
							buf->Produce(size);

							// position
							math::Vector3 position = { 0.0f, 0.0f, 1.0f }; // default position
							position = avatar->getPosition();
							size = snprintf(buf->writer(), buf->FreeBufSize(), "position=(%ff, %ff, %ff)\n", position.x, position.y, position.z);
							buf->Produce(size);

							// model
							std::string model = (std::string)(avatar->getValue("model"));
							std::string gender = (std::string)(avatar->getValue("gender"));
							size = snprintf(buf->writer(), buf->FreeBufSize(), "model=\"%s\"\ngender=\"%s\"\n\n", model.c_str(), gender.c_str());
							buf->Produce(size);
						}
						return true;
						});
				}
				return true;
				});
			stream->Open();
			stream->Write(buf->reader(), buf->Remaining());
			stream->Close();
			delete buf;
			handler(0);
		}
		});
}
