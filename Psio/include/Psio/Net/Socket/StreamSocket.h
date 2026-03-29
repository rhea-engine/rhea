#ifndef PSIO_NET_SOCKET_STREAMSOCKET_H
#define PSIO_NET_SOCKET_STREAMSOCKET_H

#include "./Socket.h"
#include "SocketAddress.h"

namespace Psio
{
	class StreamSocket : public Socket
	{
	public:
		~StreamSocket() override = default;

		// connect to ip address
		virtual int Connect(SocketAddress& addr, bool blocking, Socket::CompletionHandler handler) = 0;

		// server functions

		typedef std::function<void(StreamSocket*, int)> ConnectHandler;

		// accept a socket connection
		virtual int Accept(ConnectHandler handler) = 0;
	};
}

#endif // !PSIO_NET_SOCKET_STREAMSOCKET_H
