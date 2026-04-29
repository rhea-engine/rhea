/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef FORTYTWO_SERVERSERVICE_H
#define FORTYTWO_SERVERSERVICE_H

#include <Rhea/Service.h>
#include <Rhea/Unit.h>
#include <Psio/Psio.h>
#include <Psio/Net/Network.h>
#include <Psio/Net/Socket/TCPSocket.h>
#include <Hermes/MessageFactory.h>
#include <Rhea/Core/Scene.h>
#include <Hermes/ServerStream.h>
#include <Rhea/Application.h>

#include "ClientConnection.h"

using rhea::Unit;

namespace rhea
{

	class ServerService : public rhea::Service
	{
		MECLASS(ServerService, rhea::Service)
	public:
		ServerService(std::string name, std::string type) :
			m_network(nullptr),
			m_scene(nullptr),
			nuid(1),
			m_port(4242),
			rhea::Service(name, type)
		{}

		void setup(rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// ready event
		void ready();

		void setNetwork(psio::Network* network) { m_network = network; }

		// set port
		void setPort(uint16_t port) { m_port = port; }

		void onConnection(std::function<void(ClientConnection*)> connection_handler) { m_on_connection = connection_handler; }

		// set the scene we control
		void setScene(rhea::Scene* scene) { m_scene = scene; }

		// handle frameupdate
		bool onFrameEnd();

		int getUID() { return nuid++; }

		// broadcast message
		// is source is set it is not sent there
		virtual void broadcast(hermes::Message* msg, ClientConnection* source);

	protected:
		// disconnect an item
		void disconnect(ClientConnection* stream);

		std::unique_ptr<psio::TCPSocket> m_server_sock;
		psio::Network* m_network;
		std::unique_ptr<hermes::MessageFactory> m_msg_factory;
		rhea::Scene* m_scene;
		std::vector<ClientConnection*> m_connections;
		std::string ip;
		uint16_t m_port;

		std::function<void(ClientConnection*)> m_on_connection;

		int nuid;
	};
}

#endif // !FORTYTWO_SERVERSERVICE_H
