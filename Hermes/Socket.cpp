#include <errno.h>
#include <stdio.h>

#include "Socket.h"

#ifdef WIN32
#define SOCK_EMPTY INVALID_SOCKET
#define SOCK_CLOSE closesocket
#define SOCK_BUF(x) (char *)(x)
#define SOCK_CONST_BUF(x) (const char *)(x)
#else
#define SOCK_EMPTY -1
#define SOCK_CLOSE ::close
#define SOCK_BUF(x) x
#define SOCK_CONST_BUF(x) x
#endif // WIN32


namespace Hermes
{
	bool Socket::m_initialized = false;

	Socket::Socket() :
		m_socket(SOCK_EMPTY),
		m_is_open(false),
		m_type(IP_ANY),
		m_family(0)
	{
		if (!m_initialized)
		{
			m_initialized = true;
#ifdef WIN32
			WSADATA wsa_data;
			int ret = WSAStartup(MAKEWORD(2, 2), &wsa_data);
			if (ret != 0) {
				printf("WSAStartup failed with error: %d\n", ret);				
			}
#endif
		}
	}

	void Socket::close()
	{
		if (m_socket != SOCK_EMPTY)
			SOCK_CLOSE(m_socket);

		m_socket = SOCK_EMPTY;
		m_is_open = false;
	}

	int Socket::getSocketError() const {
#ifdef WIN32
		int err = WSAGetLastError();
		return err;
#else
		return errno;		
#endif
	}

	int Socket::receive(uint8_t* buffer, int buflen, int& read)
	{
		if (!m_is_open)
			return -1;

		read = ::recv(m_socket, SOCK_BUF(buffer), buflen, 0);

		if (read < 0) {
			// TODO ckeck for wouldblock
			return -1;
		}
		return read;
	}

	int Socket::send(const uint8_t* buffer, int buflen, int& sent) {
		
		int flags = 0;

		sent = ::send(m_socket, SOCK_CONST_BUF(buffer), buflen, flags);

		if (sent < 0) {
			// TODO ckeck for wouldblock
			return -1;
		}
		return sent;
	}

	Socket::~Socket()
	{
		close();
	}
	
	int Socket::open(IP_SOCKET_TYPE ip_socket_type, int ip_version)
	{
		m_family = (ip_version == 4) ? AF_INET : AF_INET6;
		int protocol = (ip_socket_type == IP_TCP) ? IPPROTO_TCP : IPPROTO_UDP;
		int type = (ip_socket_type == IP_TCP) ? SOCK_STREAM : SOCK_DGRAM;
		m_type = ip_socket_type;		

		m_socket = socket(m_family, type, protocol);

		if (m_socket < 0)
		{
			printf("Could not create socket. Reason: %d", getSocketError());
			return -1;
		}
		else {
			m_is_open = true;
			return 0;
		}
	}

	int Socket::bind(IPAddress& ip_addr, uint16_t port)
	{
		sockaddr_storage addr;
		size_t addr_size = setAddrStorage(&addr, ip_addr, port, m_family);
		
		if (::bind(m_socket, (struct sockaddr*) &addr, (int)addr_size) != 0) {
			printf("Failed to bind socket: %d\n", getSocketError());
			close();
			return -1;
		}

		return 0;
	}

	int Socket::listen(int max_pending)
	{
		if (::listen(m_socket, max_pending) != 0) {			
			printf("Failed to listen from socket: %d", getSocketError());
			close();
			return -1;
		};
		return 0;
	}

	int Socket::setReuseAddress(bool enabled) {

		int par = enabled ? 1 : 0;
		if (setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, SOCK_CONST_BUF(&par), sizeof(int)) < 0) {
			printf("Unable to set socket reuse_addr option.\n");
			return -1;
		}
		return 0;
	}

	int Socket::poll(bool in, bool out, int timeout)
	{
#ifdef WIN32
		fd_set r_s, w_s, x_s;
		fd_set* r_p = NULL;
		fd_set* w_p = NULL;
		// prepare fd_sets
		FD_ZERO(&r_s);
		FD_ZERO(&w_s);
		FD_ZERO(&x_s);
		
		// set the timeout if positive
		struct timeval t_o = { timeout, 0 };
		struct timeval* tp = NULL;
		if (timeout >= 0)
			tp = &t_o;

		// set the bits in the sets
		FD_SET(m_socket, &x_s);
		if (in) {
			FD_SET(m_socket, &r_s);
			r_p = &r_s;
		}
		if (out) {
			FD_SET(m_socket, &w_s);
			w_p = &w_s;
		}

		int ret = select(1, r_p, w_p, &x_s, tp);
		if (ret == SOCKET_ERROR)
		{
			return -1;
		}

		if (ret == 0)
		{
			return 0;
		}

		if (FD_ISSET(m_socket, &x_s)) {			
			printf("Exception when polling socket: %d\n", getSocketError());
			return -1;
		}

		if (r_p && FD_ISSET(m_socket, r_p))
			return 1;
		if (w_p && FD_ISSET(m_socket, w_p))
			return 1;
#else
		struct pollfd p_fd;
		p_fd.fd = m_socket;
		p_fd.events = 0;
		p_fd.revents = 0;

		if (in)
			p_fd.events |= POLLIN;
			
		if (out)
			p_fd.events |= POLLOUT;
			break;
		
		int ret = ::poll(&p_fd, 1, timeout);

		if (ret < 0 || p_fd.revents & POLLERR) {
			printf("Error when polling socket: %d\n", getSocketError());
			return -1;
		}

		if (ret == 0)
			return 0;

		return 1;
#endif
		return 0;
	}

	int Socket::setBlocking(bool enabled) {		

		int ret = 0;
#ifdef WIN32
		unsigned long par = enabled ? 0 : 1;
		ret = ioctlsocket(m_socket, FIONBIO, &par);
#else
		int opts = fcntl(m_socket, F_GETFL);
		if (enabled)
			ret = fcntl(m_socket, F_SETFL, opts & ~O_NONBLOCK);
		else
			ret = fcntl(m_socket, F_SETFL, opts | O_NONBLOCK);
#endif

		if (ret != 0) {
			printf("Unable to change non-block mode\n");
			return -1;
		}
		return 0;
	}

	Socket* Socket::accept(IPAddress& ip, uint16_t& port)
	{
		struct sockaddr_storage remote_address;
		socklen_t size = sizeof(remote_address);
		SOCKET_T fd = ::accept(m_socket, (struct sockaddr*) & remote_address, &size);
		if (fd == SOCK_EMPTY) {
			printf("Error when accepting socket connection:%d\n", getSocketError());
			return nullptr;
		}

		setAddrNtoH(&remote_address, ip, port);

		Socket* new_socket = new Socket();
		new_socket->m_socket = fd;
		new_socket->m_type = m_type;
		new_socket->setBlocking(false);
		new_socket->m_is_open = true;
		return new_socket;		
	}

	size_t Socket::setAddrStorage(struct sockaddr_storage* addr, IPAddress& ip, uint16_t port, int ip_family) {

		memset(addr, 0, sizeof(struct sockaddr_storage));
		if (ip_family == AF_INET6) { // IPv6 socket

			struct sockaddr_in6* addr6 = (struct sockaddr_in6*)addr;
			addr6->sin6_family = AF_INET6;
			addr6->sin6_port = htons(port);
			if (ip.isValid()) {
				memcpy(&addr6->sin6_addr.s6_addr, ip.get_ipv6(), 16);
			}
			else {
				addr6->sin6_addr = in6addr_any;
			}
			return sizeof(sockaddr_in6);
		}
		else { // IPv4 socket

			struct sockaddr_in* addr4 = (struct sockaddr_in*)addr;
			addr4->sin_family = AF_INET;
			addr4->sin_port = htons(port);

			if (ip.isValid()) {
				memcpy(&addr4->sin_addr.s_addr, ip.get_ipv4(), 4);
			}
			else {
				addr4->sin_addr.s_addr = INADDR_ANY;
			}

			return sizeof(sockaddr_in);
		}
	}

	void Socket::setAddrNtoH(struct sockaddr_storage* addr, IPAddress& r_ip, uint16_t& r_port) {

		if (addr->ss_family == AF_INET) {

			struct sockaddr_in* addr4 = (struct sockaddr_in*)addr;
			r_ip.set_ipv4((uint8_t*)&(addr4->sin_addr.s_addr));
			r_port = ntohs(addr4->sin_port);

		}
		else if (addr->ss_family == AF_INET6) {

			struct sockaddr_in6* addr6 = (struct sockaddr_in6*)addr;
			r_ip.set_ipv6(addr6->sin6_addr.s6_addr);
			r_port = ntohs(addr6->sin6_port);
		};
	}
}