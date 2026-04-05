#include "Rhea/Network/ClientService.h"
#include <Psio/Net/Socket/TCPSocket.h>
#include <Psio/Log.h>
#include <Hermes/ClientStream.h>
#include <Hermes/Opcode.h>
#include <Hermes/Messages/MessageCreate.h>
#include <Rhea/Application.h>
#include <Rhea/Platform/PlatformService.h>

namespace rhea
{
	void ClientService::ready()
	{
		rhea::PlatformService* psio = static_cast<rhea::PlatformService*>(m_app->getService("platform"));
		m_network = psio->getEngine()->getNet();
		m_worldname = getValueString("world", "");
		m_app->registerService("hermesclient", this);
		m_msg_factory = std::make_unique<hermes::MessageFactory>();
		m_msg_factory->registerCoreMessages();
	}

	void ClientService::Connect(std::string host, ConnectionHandler handler)
	{
		psio::StreamSocket* m_sock = m_network->CreateTCPSocket();
		psio::SocketAddress addr;
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

	void ClientService::OnTCPConnect(psio::StreamSocket* sock)
	{
		std::unique_ptr<psio::StreamSocket> new_sock;
		new_sock.reset(sock);
		hermes::Stream* stream = new hermes::Stream(std::move(new_sock), m_msg_factory.get());
		int ret = stream->Connect(m_worldname, [&, stream](int err) {
			if (!err) {
				m_handler(stream, 0);
			}
			else {
				LOG(LOG_ERROR) << "Connection failed(2): " << err;
				delete stream;
				m_handler(nullptr, err);
			}
			});
		if (ret < -1) {
			LOG(LOG_ERROR) << "Connection failed: " << ret;
			delete stream;
			m_handler(nullptr, ret);
		}
	}
}