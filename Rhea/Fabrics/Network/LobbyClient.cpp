#include "LobbyClient.h"
#include <Psio/Log.h>
#include <Rhea/Core/Avatar.h>
#include <Rhea/UnitTree.h>
#include <Hermes/Messages/MessageEnterWorld.h>
#include <Hermes/Messages/MessageCreateAvatar.h>

namespace Rhea
{
	void LobbyClient::Initialize()
	{
		Hermes::MessageFactory* factory = m_stream->getMessageFactory();
		if (!factory)
		{
			LOG(LOG_ERROR) << "No message factory found.";
			return;
		}

		factory->registerMessageHandler<Hermes::MessageAvatarList>(HERMES_MSG_AVATARLIST,
			[&](uint16_t opcode)->bool {
				return m_state == CONN_LOBBY;
			},
			std::bind(&LobbyClient::onAvatarlistMessage, this, std::placeholders::_1)
		);

		factory->registerMessageHandler<Hermes::Message>(HERMES_MSG_CREATEAVATAR_FAILED,
			[&](uint16_t opcode)->bool {
				return m_state != CONN_LOBBY;
			},
			std::bind(&LobbyClient::onCreateAvatarFailedMessage, this, std::placeholders::_1)
		);

		factory->registerMessageHandler<Hermes::Message>(HERMES_MSG_ENTER_FAILED,
			[&](uint16_t opcode)->bool {
				return m_state != CONN_LOBBY;
			},
			std::bind(&LobbyClient::onEnterWorldFailedMessage, this, std::placeholders::_1)
		);
	}

	void LobbyClient::EnterWorld(std::string avatar_name)
	{
		// send enter with the given avatar name
		Hermes::MessageEnterWorld msg_enter;
		msg_enter.avatar = avatar_name;
		m_stream->sendMessage(&msg_enter, nullptr);
	}

	void LobbyClient::CreateAvatar(std::string name, std::string type_name)
	{
		Hermes::MessageCreateAvatar msg(HERMES_MSG_CREATEAVATAR);
		msg.avatar.name = name;
		msg.avatar.type_name = type_name;
		m_stream->sendMessage((Hermes::Message*)&msg, nullptr);
	}

	void LobbyClient::Start()
	{
		m_state = CONN_LOBBY;

		// send get avatar list
		Hermes::Message msg_get_avatar_list(HERMES_MSG_GET_AVATARLIST);
		m_stream->sendMessage(&msg_get_avatar_list, nullptr);
	}

	void LobbyClient::onAvatarlistMessage(Hermes::MessageAvatarList*msg)
	{
		// fill up avatar list of scene
		if (!m_owner)
			return;
		Rhea::Unit* avatar_list = doCast<Rhea::Unit>(m_owner->getChild("avatarlist"));
		if (!avatar_list) {
			avatar_list = new Rhea::Unit("avatarlist", "//meworla/avatarlist");
			avatar_list->setup(m_app);
			m_owner->addChild(avatar_list);
		}
		if (msg->avatars.size()) {
			for (auto it = msg->avatars.begin(); it != msg->avatars.end(); it++) {
				Rhea::Avatar* avatar = (Rhea::Avatar*)m_app->getUnitTree()->createUnit((*it).avatar, "//meworla/avatar");
				avatar->setModel((*it).type_name);
				avatar_list->addChild(avatar);
			}
		}
		m_avatarlist_handler((int)msg->avatars.size());
	}

	void LobbyClient::onCreateAvatarFailedMessage(Hermes::Message* msg)
	{
		// avatar entering failed
		m_state = CONN_LOBBY;

		m_create_failed_handler(0);
	}

	void LobbyClient::onEnterWorldFailedMessage(Hermes::Message* msg)
	{
		// avatar entering failed
		m_state = CONN_LOBBY;

		// inform user
		m_enter_failed_handler(0);
	}
}