#ifndef PSIO_NET_SOCKETADDRESS_H
#define PSIO_NET_SOCKETADDRESS_H

#include <string>

#include "../Network.h"

namespace Psio
{
	class SocketAddress
	{
	public:
		SocketAddress();
		bool SetHost(const std::string& ip_addr, unsigned short port);
		bool SetHost(const std::string& ip_addr);
		std::string& GetHost() { return m_host; }

		// get adress family
		sa_family_t getFamily() const;

		// get address info
		struct sockaddr& GetAddressInfo() { return m_addr; }
		const struct sockaddr& GetAddressInfo() const { return m_addr; }

	private:
		union {
			sockaddr m_addr;
			sockaddr_in m_addr4;
			sockaddr_in6 m_addr6;
			sockaddr_storage m_storage;
		};
		bool m_ipv4;
		bool m_ipv6;
		std::string m_host;
	};
}

#endif // !PSIO_NET_SOCKETADDRESS_H
