#include "Rhea/Network/LobbyServer.h"
#include <Psio/Log.h>
#include <Hermes/Messages/MessageAvatarList.h>
#include <Rhea/Core/Avatar.h>
#include <Hermes/Messages/MessageEnterFailed.h>

namespace rhea
{
	void LobbyServer::Initialize()
	{
		hermes::MessageFactory* factory = m_hermes->getMessageFactory();
		if (!factory) {
			LOG(LOG_ERROR) << "No message factory found";
			return;
		}

		factory->registerMessageHandler<hermes::Message>(HERMES_MSG_GET_AVATARLIST,
			[&](uint16_t opcode)->bool {
				return m_state == CONN_LOBBY;
			},
			std::bind(&LobbyServer::onGetAvatarListMessage, this, std::placeholders::_1)
		);

		factory->registerMessageHandler<hermes::MessageCreateAvatar>(HERMES_MSG_CREATEAVATAR,
			[&](uint16_t opcode)->bool {
				return m_state == CONN_LOBBY;
			},
			std::bind(&LobbyServer::onCreateAvatarMessage, this, std::placeholders::_1)
		);

		// world messages

		factory->registerMessageHandler<hermes::MessageEnterWorld>(HERMES_MSG_ENTERWORLD,
			[&](uint16_t opcode)->bool {
				return m_state == CONN_LOBBY;
			},
			std::bind(&LobbyServer::onEnterWorldMessage, this, std::placeholders::_1)
		);
	}

	void LobbyServer::onGetAvatarListMessage(hermes::Message* msg)
	{
		// send avatar list (which may be empty)

		// fill the avatars in the avatarlist message
		hermes::MessageAvatarList msg_al;
		for (auto avatar : avatars)
		{
			std::string pathName = avatar->getPathName();
			std::string typeName = avatar->getTypeName();
			msg_al.addAvatar(pathName, typeName);
		}
	
		m_state = CONN_LOBBY;
		m_hermes->sendMessage(&msg_al, nullptr);
	}

	void LobbyServer::onCreateAvatarMessage(hermes::MessageCreateAvatar* msg_avatar)
	{
		if (m_on_create_avatar)
			m_on_create_avatar(msg_avatar->avatar.name, msg_avatar->avatar.type_name, [&](Avatar* avatar) {
			if (avatar && m_on_enter_world) {
				avatars.push_back(avatar);
				if (m_on_enter_world(avatar)) {
					// error
					hermes::MessageEnterFailed errmsg;
					errmsg.reason = 1;
					m_hermes->sendMessage((hermes::Message*)&errmsg, nullptr);
				}
			}
			else {
				// avatar could not be added - report failure				
				hermes::MessageEnterFailed errmsg(HERMES_MSG_CREATEAVATAR_FAILED);
				errmsg.reason = 1;
				m_state = CONN_LOBBY;
				m_hermes->sendMessage((hermes::Message*)&errmsg, nullptr);
			}
				});
		else {
			// set some defaults?
		}		
	}

	void LobbyServer::onEnterWorldMessage(hermes::MessageEnterWorld* msg)
	{
		// get the avatar with that name
		rhea::Avatar* avatar = nullptr;
		for (int i = 0; i < avatars.size(); i++)
		{
			if (avatars[i]->getPathName() == msg->avatar)
				avatar = avatars[i];
		}

		if (!avatar) {
			LOG(LOG_ERROR) << "Avatar " << msg->avatar << " not found";
			// send error message
			hermes::MessageEnterFailed errmsg;
			errmsg.reason = 1;
			m_state = CONN_LOBBY;
			m_hermes->sendMessage((hermes::Message*)&errmsg, nullptr);
			return;
		}
	
		if (m_on_enter_world(avatar)) {
			// error
			hermes::MessageEnterFailed errmsg;
			errmsg.reason = 1;
			m_hermes->sendMessage((hermes::Message*)&errmsg, nullptr);
		}
	}
}