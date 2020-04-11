#ifndef Hermes_TCP_SERVER_H
#define Hermes_TCP_SERVER_H

#include "Socket.h"
#include "IPAddress.h"

namespace Hermes
{
	class TcpStream;

	class TcpServer
	{
	public:
		TcpServer();
		// start a server on the address and port. if address is invalid listen on all of them
		int start(IPAddress& address, uint16_t port);
		// check if connection is available, returns true if yes
		bool connectionAvailable();
		// accept a connection and return a tcp stream. returns NULL if accept fails
		TcpStream* accept();

	protected:
		Socket *m_socket;
	};
}

#endif // !Hermes_TCP_SERVER_H
