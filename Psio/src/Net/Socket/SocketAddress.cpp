/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Psio/Net/Socket/SocketAddress.h>
#include <cassert>
#include <cstring>

namespace psio
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

	void SocketAddress::SetAddrNtoH(struct sockaddr_storage* addr, IPAddress& r_ip, uint16_t& r_port)
	{
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

	bool SocketAddress::SetAddress(sockaddr_storage* addr)
	{
		m_storage = *addr;
		SetAddrNtoH(&m_storage, m_ip, m_port);
		return true;
	}

	
	sa_family_t SocketAddress::getFamily() const
	{
		return m_storage.ss_family;
	}
}