#ifndef RHEA_CLIENTSERVICE_H
#define RHEA_CLIENTSERVICE_H

#include <Rhea/Service.h>
#include <Rhea/Unit.h>
#include <Rhea/Core/Scene.h>
#include <Psio/Psio.h>
#include <Psio/Net/Network.h>
#include <Psio/Net/Socket/StreamSocket.h>
#include <Hermes/Stream.h>
#include <Hermes/ClientStream.h>
#include <Hermes/MessageFactory.h>
#include "WorldClient.h"

using rhea::Unit;

namespace rhea
{
	class ClientService : public rhea::Service
	{
		MECLASS(ClientService, rhea::Service)
	public:
		ClientService(std::string name, std::string type) :
			m_network(nullptr),
			rhea::Service(name, type)
		{}

		// init event
		void ready();

		// connecting
		typedef std::function<void(Hermes::Stream* stream, int)> ConnectionHandler;
		void Connect(std::string host, ConnectionHandler handler);		

		void setNetwork(Psio::Network* network) { m_network = network; }

	protected:
		Psio::Network* m_network;
		std::unique_ptr<Hermes::MessageFactory> m_msg_factory;		
		std::string m_worldname;
		ConnectionHandler m_handler;

		void OnTCPConnect(Psio::StreamSocket* sock);
	//	void OnConnect();
	};

}
#endif // !RHEA_CLIENTSERVICE_H
