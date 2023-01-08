#include "SocketAddress.h"
#include <cassert>

namespace Psio
{
	SocketAddress::SocketAddress() : 
		m_ipv4(true),
		m_ipv6(true)
	{
	}

	bool SocketAddress::SetHost(const std::string& ip_addr, unsigned short port)
	{
		if (SetHost(ip_addr)) {
			m_addr4.sin_port = htons(port);
			return true;
		}
		else
			return false;
	}
	
	bool SocketAddress::SetHost(const std::string& ip_addr)
	{
		struct addrinfo hints, * res = nullptr;
		memset(&hints, 0, sizeof(hints));

		m_host = ip_addr;

		hints.ai_flags = AI_ADDRCONFIG;
		hints.ai_family = m_ipv4 ? (m_ipv6 ? AF_UNSPEC : AF_INET) : AF_INET6;

		if (getaddrinfo(ip_addr.c_str(), nullptr, &hints, &res)) {
			return false;
		}

		assert(res->ai_addrlen <= sizeof(m_storage));
		memcpy(&m_storage, res->ai_addr, res->ai_addrlen);
		freeaddrinfo(res);

		return true;
	}
	
	sa_family_t SocketAddress::getFamily() const
	{
		return m_storage.ss_family;
	}
}