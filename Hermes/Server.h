#ifndef Hermes_NET_SERVER
#define Hermes_NET_SERVER

#include <stdio.h>
#include <list>
#include "IPAddress.h"
#include "Task.h"

namespace Hermes
{
	class TcpServer;
	class IPAddress;
	class Connection;
	class IRequestHandlerFactory;
	class IConnectionHandler;

	class Server : public Task
	{
	public:
		Server(bool tcp, bool udp);
		int listen(IPAddress& ip, uint16_t port);
		void setRequestHandlerFactory(IRequestHandlerFactory* req_handler_factory) { m_req_handler_factory = req_handler_factory; }
		// add handler for connection
		void addConnectionHandler(IConnectionHandler* conn_handler) { m_conn_handlers.push_back(conn_handler); }
		void run();
	protected:
		TcpServer* m_tcp_server = NULL;
		bool m_tcp = true;
		bool m_udp = true;
		std::list<Connection*> m_connections;
		IRequestHandlerFactory* m_req_handler_factory;
		std::list<IConnectionHandler*> m_conn_handlers;
	};
}

#endif // !Hermes_NET_SERVER
