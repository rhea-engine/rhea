#ifndef Hermes_TCP_STREAM_H
#define Hermes_TCP_STREAM_H

#include <stdint.h>
#include "Socket.h"
#include "IPAddress.h"

namespace Hermes
{
	class TcpStream
	{
	public:
		TcpStream() :
			m_socket(NULL),
			m_remote_addr({ IPAddress(), 0 })
		{}
		~TcpStream();
		// build tcp stream from accepted socket, storing the remote address
		int accept(Socket* socket, IPAddress remote_ip, uint16_t port);
		// read up to n bytes from stream - this does not block
		int read(uint8_t* buffer, int bytes, int& received);
		// read up to n bytes to stream - this does not block
		int write(uint8_t* buffer, int bytes, int& sent);
		// return if input is pending
		bool inputPending();
		// close the connection
		void close();

		// return the current remote ip or null if not connected
		IPAddress* get_remote_ip() { return &m_remote_addr.ip; };
		// return the current connection port
		uint16_t get_port() { return m_remote_addr.port; }

	protected:
		Socket* m_socket;
		struct {
			IPAddress ip;
			uint16_t port;
		} m_remote_addr;
	};
}
#endif // !Hermes_TCP_STREAM_H
