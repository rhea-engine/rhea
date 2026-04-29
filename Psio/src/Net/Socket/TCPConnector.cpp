/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Psio/Net/Socket/TCPConnector.h>
#include <Psio/Net/Socket/TCPSocket.h>
#include <Psio/Net/Socket/SocketAddress.h>

namespace psio
{
	TCPConnector::TCPConnector(Network* net, std::string &host, unsigned short port) :
		m_net(net),
		m_host(host),
		m_port(port)
	{
	}

	TCPConnector::~TCPConnector()
	{
	}
	
	int TCPConnector::Start(Network::CompletionHandler handler)
	{
		m_socket.reset(m_net->CreateTCPSocket());
		SocketAddress m_s_addr;

		if (m_host == "")
			m_s_addr.SetHost("localhost", m_port);
		else
			m_s_addr.SetHost(m_host, m_port);
		int ret = m_socket->Connect(m_s_addr, false, [handler](int result) -> void {
			handler(result);
			});
		return ret;
	}
}
