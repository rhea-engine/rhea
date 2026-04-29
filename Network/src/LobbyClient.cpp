/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Network/LobbyClient.h"
#include <Psio/Log.h>
#include <Rhea/Core/Avatar.h>
#include <Rhea/UnitTree.h>
#include <Hermes/Messages/MessageEnterWorld.h>
#include <Hermes/Messages/MessageCreateAvatar.h>

namespace rhea
{
	void LobbyClient::Initialize()
	{
		hermes::MessageFactory* factory = m_stream->getMessageFactory();
		if (!factory)
		{
			LOG(LOG_ERROR) << "No message factory found.";
			return;
		}

		factory->registerMessageHandler<hermes::MessageAvatarList>(HERMES_MSG_AVATARLIST,
			[&](uint16_t opcode)->bool {
				return m_state == CONN_LOBBY;
			},
			std::bind(&LobbyClient::onAvatarlistMessage, this, std::placeholders::_1)
		);

		factory->registerMessageHandler<hermes::Message>(HERMES_MSG_CREATEAVATAR_FAILED,
			[&](uint16_t opcode)->bool {
				return m_state != CONN_LOBBY;
			},
			std::bind(&LobbyClient::onCreateAvatarFailedMessage, this, std::placeholders::_1)
		);

		factory->registerMessageHandler<hermes::Message>(HERMES_MSG_ENTER_FAILED,
			[&](uint16_t opcode)->bool {
				return m_state != CONN_LOBBY;
			},
			std::bind(&LobbyClient::onEnterWorldFailedMessage, this, std::placeholders::_1)
		);
	}

	void LobbyClient::EnterWorld(std::string avatar_name)
	{
		// send enter with the given avatar name
		hermes::MessageEnterWorld msg_enter;
		msg_enter.avatar = avatar_name;
		m_stream->sendMessage(&msg_enter, nullptr);
	}

	void LobbyClient::CreateAvatar(std::string name, std::string type_name)
	{
		hermes::MessageCreateAvatar msg(HERMES_MSG_CREATEAVATAR);
		msg.avatar.name = name;
		msg.avatar.type_name = type_name;
		m_stream->sendMessage((hermes::Message*)&msg, nullptr);
	}

	void LobbyClient::Start()
	{
		m_state = CONN_LOBBY;

		// send get avatar list
		hermes::Message msg_get_avatar_list(HERMES_MSG_GET_AVATARLIST);
		m_stream->sendMessage(&msg_get_avatar_list, nullptr);
	}

	void LobbyClient::onAvatarlistMessage(hermes::MessageAvatarList*msg)
	{
		// fill up avatar list of scene
		if (!m_owner)
			return;
		rhea::Unit* avatar_list = doCast<rhea::Unit>(m_owner->getChild("avatarlist"));
		if (!avatar_list) {
			avatar_list = new rhea::Unit("avatarlist", "//meworla/avatarlist");
			avatar_list->setup(m_app);
			m_owner->addChild(avatar_list);
		}
		if (msg->avatars.size()) {
			for (auto it = msg->avatars.begin(); it != msg->avatars.end(); it++) {
				rhea::Avatar* avatar = (rhea::Avatar*)m_app->getUnitTree()->createUnit((*it).avatar, "//meworla/avatar");
				avatar->setModel((*it).type_name);
				avatar_list->addChild(avatar);
			}
		}
		m_avatarlist_handler((int)msg->avatars.size());
	}

	void LobbyClient::onCreateAvatarFailedMessage(hermes::Message* msg)
	{
		// avatar entering failed
		m_state = CONN_LOBBY;

		m_create_failed_handler(0);
	}

	void LobbyClient::onEnterWorldFailedMessage(hermes::Message* msg)
	{
		// avatar entering failed
		m_state = CONN_LOBBY;

		// inform user
		m_enter_failed_handler(0);
	}
}