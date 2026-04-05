#ifndef PSIO_IP_ADDRESS_H
#define PSIO_IP_ADDRESS_H

#include <stdint.h>

namespace psio
{
	class IPAddress
	{
	public:		
		IPAddress();
		// returns wheter ip address is valid. A wildcard is always invalid
		bool isValid() { return m_valid; }
		// return buffer of ipv4 address part
		const uint8_t* get_ipv4() const;
		// return buffer of full ipv6 address
		const uint8_t* get_ipv6() const;
		bool is_ipv4() { return m_family == 4; }
		// set the address as ipv4
		void set_ipv4(const uint8_t* ip);
		// set the address as ipv6
		void set_ipv6(const uint8_t* ip);
		// return the ip as char string s, even if not set
		char* get_s(char* s);

	protected:
		uint8_t m_addr[16];
		bool m_valid;
		int m_family;
	};
}

#endif // !PSIO_IP_ADDRESS_H
