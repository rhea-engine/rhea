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
		m_worldname = getValue("world", "");
		m_app->registerService("hermesclient", this);
		m_msg_factory = std::make_unique<Hermes::MessageFactory>();
		m_msg_factory->registerCoreMessages();
	}

	void ClientService::Connect(std::string host, ConnectionHandler handler)
	{
		Psio::StreamSocket* m_sock = m_network->CreateTCPSocket();
		Psio::SocketAddress addr;
		addr.SetHost("127.0.0.1", 4242);

		// fill message factory
		m_handler = handler;

		int ret = m_sock->Connect(addr, false, [&,m_sock](int err) {
			if (!err) {
				OnTCPConnect(m_sock);				
			}
			else {
				LOG(LOG_ERROR) << "Connection failed: " << err;
				m_handler(nullptr, err);
				delete m_sock;
			}
			});
		if (ret < -1) 
		{
			LOG(LOG_ERROR) << "Connection failed: " << ret;
			handler(nullptr, ret);
			delete m_sock;
		}
	}

	void ClientService::OnTCPConnect(Psio::StreamSocket* sock)
	{
		std::unique_ptr<Psio::StreamSocket> new_sock;
		new_sock.reset(sock);
		Hermes::ClientStream* hermes = new Hermes::ClientStream(std::move(new_sock), m_msg_factory.get());
		int ret = hermes->Connect(m_worldname, [&, hermes](int err) {
			if (!err) {
				m_handler(hermes, 0);
			}
			else {
				LOG(LOG_ERROR) << "Connection failed(2): " << err;
				delete hermes;
				m_handler(nullptr, err);
			}
			});
		if (ret < -1) {
			LOG(LOG_ERROR) << "Connection failed: " << ret;
			delete hermes;
			m_handler(nullptr, ret);
		}
	}

	/*
	void ClientService::OnConnect()
	{
		WorldClient* client = new WorldClient();
		client->setStream(std::move(m_hermes));
		client->setup(m_app);
		m_handler(client, 0);
	}
	*/
}