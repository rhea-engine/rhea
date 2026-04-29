/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef MEWORLA_LOBBY_CONNECT_H
#define MEWORLA_LOBBY_CONNECT_H

#include <Rhea/Application.h>
#include <Rhea/Fabrics/Network/ClientService.h>
#include <Rhea/GUI/Widget.h>
#include <Rhea/Fabrics/Network/LobbyClient.h>
#include <Hermes/ClientStream.h>

namespace Meworla
{
	class CredentialsService;

	class ClientConnect : public rhea::Unit
	{
		MECLASS(ClientConnect, Unit)
	public:
		ClientConnect(std::string path_name, std::string type_name) :
			m_connection_state(0),
			m_error(0),
			m_net(nullptr),
			m_info_w(nullptr),
			m_error_w(nullptr),
			m_cred_srv(nullptr),
			m_trust_w(nullptr),
			m_register_w(nullptr),
			m_in_world(false),
			rhea::Unit(path_name, type_name)
		{}

		// start the connection
		bool onDisplay(rhea::Event* ev, rhea::Unit* unit);

		// abort connection
		bool onAbort(rhea::Event* ev, rhea::Unit* unit);

		// retry connection
		bool onRetry(rhea::Event* ev, rhea::Unit* unit);

		// state transition
		void setScene(rhea::Scene* scene) { if (m_world_client) m_world_client->setScene(scene); }
		void CreateAvatar(std::string name, std::string type_name);
		void EnterWorld(std::string avatar_name);

		// login
		bool onLoginTrust(rhea::Event* ev, rhea::Unit* unit);
		bool onRegister(rhea::Event* ev, rhea::Unit* unit);
		bool onRegisterFailed(rhea::Event* ev, rhea::Unit* unit);
		bool onCallRegister(rhea::Event* ev, rhea::Unit* unit);

		bool onSetUsername(rhea::Event* ev, rhea::Unit* unit);

		void doConnect();
		void doError();
		void ready();
		void doLogin();

	protected:
		void OfferAuthMethods();

		int m_connection_state;
		int m_error;
		Rhea::ClientService* m_net;
		rhea::Widget* m_info_w;
		rhea::Widget* m_error_w;
		rhea::Widget* m_trust_w;
		rhea::Widget* m_register_w;

		bool m_in_world;

		void onConnected();
		void onLogin();

		// protocol handlers
		std::unique_ptr<hermes::Stream> m_stream;
		std::unique_ptr<hermes::ClientStream> m_auth_client;
		std::unique_ptr<rhea::WorldClient> m_world_client;
		std::unique_ptr<rhea::LobbyClient> m_lobby_client;

		// credentials
		CredentialsService* m_cred_srv;
		std::string m_username;
		std::string m_name;
	};
}

#endif // !MEWORLA_LOBBY_CONNECT_H
