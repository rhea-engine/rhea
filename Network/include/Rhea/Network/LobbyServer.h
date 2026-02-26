#ifndef RHEA_NETWORK_LOBBY_SERVER_H
#define RHEA_NETWORK_LOBBY_SERVER_H

#include <Hermes/Messages/MessageEnterWorld.h>
#include <Hermes/Messages/MessageMove.h>
#include <Hermes/Messages/MessageTurn.h>
#include <Hermes/Messages/MessageCreateAvatar.h>
#include <Hermes/Stream.h>
#include <Rhea/Core/Avatar.h>

namespace Rhea
{
	class LobbyServer
	{
	public:
		LobbyServer(Hermes::Stream* hermes) :
			m_hermes(hermes),
			m_state(CONN_NONE)
		{}
		void Initialize();

		// set active
		void Start() { m_state = CONN_LOBBY; }
		void Stop() { m_state = CONN_NONE; }

		// avatar list
		std::vector<Rhea::Avatar*> avatars;

		// set the create avatar callback
		void setOnCreateAvatar(std::function<void(std::string, std::string, std::function<void(Avatar*)>)> on_create_avatar) { m_on_create_avatar = on_create_avatar; }
		// set on enter world
		void setOnEnterWorld(std::function<int(Avatar*)> on_enter_world) { m_on_enter_world = on_enter_world; }

	protected:
		Hermes::Stream* m_hermes;

		enum CONN_STATE {
			CONN_NONE,
			CONN_LOBBY,
			CONN_CLOSE
		};

		CONN_STATE m_state;

		void onGetAvatarListMessage(Hermes::Message* msg);
		void onCreateAvatarMessage(Hermes::MessageCreateAvatar* msg);
		void onEnterWorldMessage(Hermes::MessageEnterWorld* msg);

		std::function<void(std::string, std::string, std::function<void(Avatar*)>)> m_on_create_avatar;
		std::function<int(Avatar*)> m_on_enter_world;
	};
}
#endif //RHEA_NETWORK_LOBBY_SERVER_H