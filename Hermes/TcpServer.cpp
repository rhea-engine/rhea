#include "TcpServer.h"
#include "TcpStream.h"

namespace Hermes
{
	TcpServer::TcpServer() :
		m_socket(new Socket())
	{

	}
	
	int TcpServer::start(IPAddress& address, uint16_t port)
	{
		int family = 0;
		if (address.isValid())
		{
			if (address.is_ipv4())
				family = 4;
			else
				family = 6;
		}
		if (m_socket->open(Socket::IP_TCP, family) != 0)
			return -1;
		if (m_socket->setBlocking(false) != 0)
			return -1;
		if (m_socket->setReuseAddress(true) != 0)
			return -1;
		if (m_socket->bind(address, port) != 0) {
			m_socket->close();
			return -1;
		}
		if (m_socket->listen() != 0) {
			m_socket->close();
			return -1;
		}

		return 0;
	}

	bool TcpServer::connectionAvailable()
	{
		if (!m_socket->is_open())
			return false;

		int ret = m_socket->poll(true, false, 0);
		if (ret <= 0)
			return false;
		else
			return true;
	}

	TcpStream* TcpServer::accept()
	{
		Socket *r_socket;
		IPAddress ip;
		uint16_t port = 0;
		r_socket = m_socket->accept(ip, port);
		TcpStream* stream = new TcpStream();
		stream->accept(r_socket, ip, port);
		return stream;
	}
}