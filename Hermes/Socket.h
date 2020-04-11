#ifndef Hermes_SOCKET_H
#define Hermes_SOCKET_H

#include <stdint.h>

#if defined(WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>

#define SOCKET_T SOCKET
#else
#include <sys/socket.h>
#define SOCKET_T int

#endif

#include "IPAddress.h"

namespace Hermes
{
	class Socket
	{
	public:

		enum IP_SOCKET_TYPE
		{
			IP_TCP,
			IP_UDP,
			IP_ANY
		};

		Socket();
		~Socket();

		// open a socket with the type (tcp or udp) and the ip version
		int open(IP_SOCKET_TYPE ip_socket_type = IP_TCP, int ip_version = 4);
		// bind to address and port
		int bind(IPAddress& ip_addr, uint16_t port);
		// listen to bound port
		int listen(int max_pending = 5);
		// set reuse address flag
		int setReuseAddress(bool enabled);
		// poll the socket for input or output with the given timeout
		int poll(bool in, bool out, int timeout);
		// set to blocking mode if true or non-blocking if false
		int setBlocking(bool enabled);
		// accept connection from listening socket and return a new socket or NULL on failure
		Socket* accept(IPAddress &ip, uint16_t &port);
		// close the internal socket
		void close();
		// returns the socket error number independant of os
		int getSocketError() const;
		// return wheter the socket is opened
		bool is_open() { return m_is_open; }
		// read a chunk of data - non-blocking. return -1 on error, 0 on success or on wouldblock
		int receive(uint8_t* buffer, int buflen, int& read);
		// send a chunk of data - non-blocking. return -1 on error, 0 on success or on wouldblock
		int send(const uint8_t* buffer, int buflen, int& sent);

	protected:
		static bool m_initialized;
		bool m_is_open;
		SOCKET_T m_socket;
		int m_family;
		IP_SOCKET_TYPE m_type;
		// store the ip and port in addr
		size_t setAddrStorage(struct sockaddr_storage* addr, IPAddress& ip, uint16_t port, int ip_family);
		// store remote address parameters in addr
		void setAddrNtoH(sockaddr_storage* addr, IPAddress& r_ip, uint16_t& r_port);
	};
}

#endif // !Hermes_SOCKET_H
