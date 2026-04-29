/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_SSLCONNECTOR_H
#define PSIO_SSLCONNECTOR_H

#include "../Network.h"
#include "SSLSocket.h"
#include "TCPSocket.h"
#include "Connector.h"
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace psio
{
	class SSLConnector : public Connector
	{
	public:
		SSLConnector(Network* net, std::string& host, unsigned short port);
		~SSLConnector();

		int Start(Network::CompletionHandler handler);

		std::unique_ptr<Socket> GetSocket() { return std::move(m_socket); }

	protected:
		Network* m_net;
		std::unique_ptr<SSLSocket> m_socket;
		std::unique_ptr<TCPSocket> m_tp_socket;
		std::string m_host;
		unsigned short m_port;
		//std::unique_ptr<SSL_CTX> m_ssl_ctx;
	};
}

#endif // !PSIO_SSLCONNECTOR_H