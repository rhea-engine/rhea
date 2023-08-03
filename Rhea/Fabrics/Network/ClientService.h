#ifndef RHEA_CLIENTSERVICE_H
#define RHEA_CLIENTSERVICE_H

#include <Rhea/Service.h>
#include <Rhea/Unit.h>
#include <Rhea/Fabrics/Core/Scene.h>
#include <Psio/Psio.h>
#include <Psio/Net/Network.h>
#include <Psio/Net/Socket/StreamSocket.h>
#include <Hermes/ClientStream.h>
#include <Hermes/MessageFactory.h>
#include "WorldClient.h"

using Rhea::Unit;

namespace Rhea
{
	class ClientService : public Rhea::Service
	{
	public:
		ClientService(std::string name, std::string type) :
			m_network(nullptr),
			Rhea::Service(name, type)
		{}

		// init event
		void onInit();

		// connecting
		typedef std::function<void(WorldClient* client, int)> ConnectionHandler;
		void connect(std::string host, ConnectionHandler handler);		

		void setNetwork(Psio::Network* network) { m_network = network; }

		// factory function
		static Unit* createType(std::string name, std::string type);

	protected:
		Psio::Network* m_network;
		std::unique_ptr<Hermes::MessageFactory> m_msg_factory;
	};

}
#endif // !RHEA_CLIENTSERVICE_H
