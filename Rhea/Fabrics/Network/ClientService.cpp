#include "ClientService.h"
#include <Psio/Net/Socket/TCPSocket.h>
#include <Psio/Log.h>
#include <Hermes/ClientStream.h>
#include <Hermes/Opcode.h>
#include <Hermes/Messages/MessageCreate.h>
#include <Rhea/Application.h>
#include <Rhea/Platform/PlatformService.h>

namespace Rhea
{
	void ClientService::ready()
	{
		Rhea::PlatformService* psio = static_cast<Rhea::PlatformService*>(m_app->getService("platform"));
		m_network = psio->getEngine()->getNet();
	}

	void ClientService::connect(std::string host, ConnectionHandler handler)
	{
		Psio::StreamSocket* m_sock = m_network->CreateTCPSocket();
		Psio::SocketAddress addr;
		addr.SetHost("127.0.0.1", 4242);

		// fill message factory
		m_msg_factory = std::make_unique<Hermes::MessageFactory>();
		m_msg_factory->registerCoreMessages();

		m_sock->Connect(addr, false, [&,m_sock,handler](int err) {
			if (!err) {
				std::unique_ptr<Psio::StreamSocket> sock;
				sock.reset(m_sock);
				std::unique_ptr<Hermes::ClientStream> hermes = std::make_unique<Hermes::ClientStream>(std::move(sock), m_msg_factory.get());
				hermes->connect(nullptr);
				WorldClient* client = new WorldClient(std::move(hermes));
				client->setup(m_app);
				handler(client, 0);
			}
			else {
				LOG(LOG_ERROR) << "Connection failed: " << err;
				handler(nullptr, err);
				delete m_sock;
			}
			});
	}

	Unit* ClientService::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new ClientService(name, type);
		return ret;
	}
}