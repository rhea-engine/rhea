#ifndef PSIO_NET_SOCKET_H
#define PSIO_NET_SOCKET_H

#include <functional>

#include "../Network.h"

namespace Psio
{
	class IOBuffer;

	class Socket
	{
	public:
		Socket() = default;
		virtual ~Socket() = default;

		typedef std::function<void(int)> CompletionHandler;

		// close the socket
		virtual bool Close() = 0;

		// get the last error. Warning: This is a static function that calls the os for last error
		static int GetLastError();

		// write to socket
		virtual int Write(IOBuffer* buf, Socket::CompletionHandler handler) = 0;

		// read from socket
		virtual int Read(IOBuffer* buf, Socket::CompletionHandler handler) = 0;
	};
}

#endif // !PSIO_NET_SOCKET_H
