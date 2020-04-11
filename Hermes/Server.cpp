#include "Server.h"
#include "TcpServer.h"
#include "Connection.h"
#include "IConnectionHandler.h"

using std::list;

namespace Hermes
{
	Server::Server(bool tcp, bool udp) :
		m_tcp(tcp),
		m_udp(udp),
		m_req_handler_factory(NULL)
	{
	}

	int Server::listen(IPAddress& ip, uint16_t port)
	{
		int ret = 0;

		if (m_tcp)
		{
			if (!m_tcp_server)
				m_tcp_server = new TcpServer();
			ret = m_tcp_server->start(ip, port);
		}
		return ret;
	}

	void Server::run()
	{
		// check for new connections
		while (m_tcp_server->connectionAvailable())
		{
			Hermes::TcpStream* s = m_tcp_server->accept();
			if (s) {

				Connection* conn = new Connection(s, m_req_handler_factory);
				// handle onConnection
				for (std::list<IConnectionHandler*>::iterator it = m_conn_handlers.begin(); it != m_conn_handlers.end(); ++it) {
					IConnectionHandler *h = *it;
					h->onConnection(conn);
				}

				m_connections.push_back(conn);
			}
		}

		// iterate over connections
		list <Connection*> ::iterator conn_it = m_connections.begin();
		list <Connection*> ::iterator conn_end = m_connections.end();
		Connection::CONN_STATE conn_state;
		while (conn_it != conn_end)
		{
			Connection* conn = *conn_it;
			conn_state = conn->iterate();
			if (conn_state != Connection::STATE_CLOSED)
				++conn_it;
			else
			{
				// connection is closed - remove it
				conn_it = m_connections.erase(conn_it);
				for (std::list<IConnectionHandler*>::iterator it = m_conn_handlers.begin(); it != m_conn_handlers.end(); ++it) {
					IConnectionHandler* h = *it;
					h->onClose(conn);
				}
				delete conn;
			}
		}

	}
}