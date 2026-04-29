/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_TCPCONNECTOR_H
#define PSIO_TCPCONNECTOR_H

#include "../Network.h"
#include "TCPSocket.h"
#include "Connector.h"

namespace psio
{
	class TCPConnector : public Connector
	{
	public:
		TCPConnector(Network *net, std::string &host, unsigned short port);
		~TCPConnector();

		int Start(Network::CompletionHandler handler);
		
		std::unique_ptr<Socket> GetSocket() { return std::move(m_socket); }

	protected:
		Network* m_net;
		std::unique_ptr<TCPSocket> m_socket;
		std::string m_host;
		unsigned short m_port;
	};
}

#endif // !PSIO_TCPCONNECTOR_H