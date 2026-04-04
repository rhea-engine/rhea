#ifndef RHEA_NETWORK_LOBBYCLIENT_H
#define RHEM_NETWORK_LOBBYCLIENT_H

#include <Hermes/Stream.h>
#include <Hermes/Messages/MessageAvatarList.h>
#include <Rhea/Unit.h>
#include <Rhea/Application.h>

namespace rhea
{
	class LobbyClient 
	{
	public:
		LobbyClient(rhea::Application* app, Hermes::Stream* stream) :
			m_app(app),
			m_stream(stream),
			m_owner(nullptr),
			m_state(CONN_NONE)
		{}
		void Initialize();

		void setAvatarOwner(rhea::Unit* owner) { m_owner = owner; } 

		void EnterWorld(std::string avatar_name);
		void CreateAvatar(std::string name, std::string type_name);

		// set callbacks
		void onAvatarlist(std::function<void(int)> handler) { m_avatarlist_handler = handler; }
		void onCreateFailed(std::function<void(int)> handler) { m_create_failed_handler = handler; }
		void onEnterFailed(std::function<void(int)> handler) { m_enter_failed_handler = handler; }

		// set active
		void Start();
		void Stop() { m_state = CONN_NONE; }

	protected:
		Hermes::Stream* m_stream;
		rhea::Unit* m_owner;
		rhea::Application* m_app;

		enum CONN_STATE {
			CONN_NONE,
			CONN_LOBBY,
			CONN_CLOSE
		};

		CONN_STATE m_state;

		void onAvatarlistMessage(Hermes::MessageAvatarList* msg);
		void onCreateAvatarFailedMessage(Hermes::Message* msg);
		void onEnterWorldFailedMessage(Hermes::Message* msg);

		std::function<void(int)> m_avatarlist_handler;
		std::function<void(int)> m_create_failed_handler;
		std::function<void(int)> m_enter_failed_handler;
	};
}
#endif // !RHEA_NETWORK_LOBBYCLIENT_H
