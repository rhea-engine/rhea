#include <stdio.h>

#include "IPAddress.h"

namespace Psio
{
	IPAddress::IPAddress() :
		m_valid(false),
		m_family(0)
	{
		for (int i = 0; i < 16; i++)
			m_addr[i] = 0;
	}

	const uint8_t* IPAddress::get_ipv4() const {
		return &(m_addr[12]);
	}

	const uint8_t* IPAddress::get_ipv6() const {
		return m_addr;
	}

	void IPAddress::set_ipv4(const uint8_t* ip)
	{
		for (int i = 0; i < 10; i++)
			m_addr[i] = 0;
		for (int i = 10; i < 12; i++)
			m_addr[i] = 256;
		for (int i = 0; i < 4; i++)
			m_addr[i+12] = ip[i];
		m_family = 4;
	}

	void IPAddress::set_ipv6(const uint8_t* ip)
	{
		for (int i = 0; i < 16; i++)
			m_addr[i] = ip[i];
		m_family = 6;
	}

	char* IPAddress::get_s(char* s)
	{
		if (m_family == 6)
			sprintf(s, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
			(int)m_addr[0], (int)m_addr[1],
				(int)m_addr[2], (int)m_addr[3],
				(int)m_addr[4], (int)m_addr[5],
				(int)m_addr[6], (int)m_addr[7],
				(int)m_addr[8], (int)m_addr[9],
				(int)m_addr[10], (int)m_addr[11],
				(int)m_addr[12], (int)m_addr[13],
				(int)m_addr[14], (int)m_addr[15]);
		else
			sprintf(s, "%d.%d.%d.%d",
			(int)m_addr[12], (int)m_addr[13],
				(int)m_addr[14], (int)m_addr[15]
			);
		return s;
	}
}