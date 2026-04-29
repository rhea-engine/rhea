/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Hermes/Stream.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Physics/PhysicsService.h>
#include <Psio/Log.h>
#include <Rhea/Network/ServerService.h>
#include <Hermes/Messages/MessageCreate.h>
#include <Rhea/Network/NetworkComponent.h>

#include "ConnectionHandler.h"
#include "Accounts/AccountService.h"
#include <Rhea/Physics/PhysicsManagerService.h>

void ConnectionHandler::Initialize(Rhea::ClientConnection* connection)
{
	m_connection = connection;

	server_stream = std::make_unique<hermes::ServerStream>(connection->getStream());
	auth_handlers = std::make_unique<AuthHandler>(m_app, m_scene, server_stream.get());
	lobby_server = std::make_unique<rhea::LobbyServer>(connection->getStream());
	world_server = std::make_unique<rhea::WorldServer>(connection->getStream());

	server_stream->Initialize();
	auth_handlers->Initialize();
	lobby_server->Initialize();
	world_server->Initialize();

	auth_handlers->onAuthSuccess([&](User* user) {
		m_user = user;
		user->Avatars([&](rhea::Avatar* avatar)->bool {
			lobby_server->avatars.push_back(avatar);
			return true;
			});
		server_stream->Stop();
		lobby_server->Start();
		});

	lobby_server->setOnCreateAvatar([&](std::string name, std::string model, std::function<void(rhea::Avatar*)> handler) {
		math::Vector3 position = { 128.0f, 0.0f, 10.0f }; // default position
		AccountService* acc_srv;
		rhea::Avatar* avatar;

		acc_srv = doCast<AccountService>(m_app->getService("accounts"));
		avatar = new rhea::Avatar(name, "//meworla/avatar");
		avatar->setModel(model);
		avatar->setPosition(position);
		acc_srv->CreateAvatar(m_user, avatar, [&, avatar](int err) {
			if (!err) {
				// enter the world with this avatar				
				handler(avatar);
			}
			else {
				// avatar could not be added - delete it
				delete avatar;
				handler(nullptr);
			}
			});
		});

	lobby_server->setOnEnterWorld([&](rhea::Avatar* avatar)->int {
		return EnterWorld(avatar);
		});

	// start with the auth handler
	server_stream->Start();
}

int ConnectionHandler::EnterWorld(rhea::Avatar* avatar)
{
	// instancing the player
	rhea::Item* player_model = doCast<rhea::Item>(m_app->getUnitTree()->find(avatar->getValueString("model")));
	if (!player_model) {
		LOG(LOG_ERROR) << " could not open avatar model " << avatar->getModel();
		return 2;
	}

	std::string avatar_name = avatar->getPathName();

	LOG(LOG_INFO) << "Avatar " << avatar_name << " entered scene";

	int i = 0;
	std::string internal_name = avatar_name;
	while (m_scene->find(internal_name)) {
		// make sure no items with the same name exist
		internal_name = avatar_name + std::string("-") + std::to_string(++i);
	}
	m_player = player_model->instance(internal_name);
	math::Vector3 position = { 128.0f, 0.0f, 10.0f }; // default position
	position = avatar->getPosition();
	m_scene->addChild(m_player);
	m_player->setPosition(position);
	m_player->setValue("avatar", avatar);

	// add a network controller
	rhea::ServerService* server_srv = (rhea::ServerService*)m_app->getService("server");
	int id = server_srv->getUID();
	m_player->addComponentUnique<rhea::NetworkComponent>(m_player, id, NET_ROLE_SERVER);

	m_scene->addItem(m_player);
	m_connection->setAuthItem(m_player);

	m_player->ready();
	DLOG(LOG_DEBUG) << "player ready";

	// get phyisc service from physics manager and initialize item
	auto* mgr = doCast<rhea::PhysicsManagerService>(m_app->getService("physics_mgr"));
	if (auto* phy_srv = mgr ? mgr->getPhysicsService() : nullptr) {
		phy_srv->initializeItem(m_player);
	}

	// inform connector
	hermes::MessageCreate msg;
	math::Vector3 pos = m_player->getTransform()->get_position()->get_worldspace();
	msg.path = internal_name;
	msg.type_name = avatar->getValueString("model");
	msg.NUID = m_player->getComponent<rhea::NetworkComponent>()->NUID();
	msg.pos[0] = pos.x;
	msg.pos[1] = pos.y;
	msg.pos[2] = pos.z;
	math::Quaternion q = m_player->getTransform()->get_position()->get_rotation();
	msg.transform[0] = q.w;
	msg.transform[1] = q.x;
	msg.transform[2] = q.y;
	msg.transform[3] = q.z;
	msg.is_player = true;
	m_connection->getStream()->sendMessage((hermes::Message*)&msg, nullptr);

	// send all items with network component
	std::vector<rhea::Item*> items = m_scene->getItems();
	for (auto item_it = items.begin(); item_it != items.end(); ++item_it) {
		if (*item_it == m_player)
			continue;
		rhea::NetworkComponent* net_com = (*item_it)->getComponent<rhea::NetworkComponent>();
		if (net_com) {
			hermes::MessageCreate item_msg;
			math::Vector3 pos = (*item_it)->getTransform()->get_position()->get_worldspace();
			item_msg.path = (*item_it)->getPathName();
			avatar = (rhea::Avatar*)(Variant*)(*item_it)->getValue("avatar");
			if (avatar) {
				item_msg.type_name = avatar->getValueString("model");
			}
			else {
				// no avatar -> TODO: check can there be normal items without avatar?
				continue;
			}
			item_msg.NUID = (*item_it)->getComponent<rhea::NetworkComponent>()->NUID();
			item_msg.pos[0] = pos.x;
			item_msg.pos[1] = pos.y;
			item_msg.pos[2] = pos.z;
			math::Quaternion q = (*item_it)->getTransform()->get_position()->get_rotation();
			item_msg.transform[0] = q.w;
			item_msg.transform[1] = q.x;
			item_msg.transform[2] = q.y;
			item_msg.transform[3] = q.z;
			item_msg.is_player = false;
			m_connection->getStream()->sendMessage((hermes::Message*)&item_msg, nullptr);
		}
	}
	// inform all other connectors
	msg.is_player = false;
	server_srv->broadcast((hermes::Message*)&msg, m_connection);

	// set state to in world
	lobby_server->Stop();
	world_server->setPlayer(m_player);
	world_server->Start();

	return 0;
}