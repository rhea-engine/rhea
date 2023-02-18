#ifndef Hermes_NET_SERVER
#define Hermes_NET_SERVER

#include <stdio.h>
#include <list>
#include <Psio/Net/Socket/IPAddress.h>
#include "Task.h"
#include <Psio/Net/Socket/TCPSocket.h>
#include <Psio/Net/Network.h>
#include <string>

namespace Hermes
{
	class Server : public Task
	{
	public:
		Server(Psio::Network* network, bool tcp, bool udp) :
			m_tcp(tcp),
			m_udp(udp),
			m_net(network)
		{
		}

		typedef std::function<void(std::unique_ptr<Psio::StreamSocket>, int)> ConnectionHandler;

		int Setup(std::string ip, uint16_t port, ConnectionHandler connection_handler);
		void Start();

		// run the loop once
		void run();
	protected:
		std::unique_ptr<Psio::StreamSocket> m_server;
		Psio::Network* m_net;
		bool m_tcp = true;
		bool m_udp = true;
		ConnectionHandler m_connection_handler;
	};
}

#endif // !Hermes_NET_SERVER
