/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_NET_SSLSOCKET_H
#define PSIO_NET_SSLSOCKET_H

#include <openssl/ssl.h>
#include <openssl/err.h>

#include "StreamSocket.h"
#include "TCPSocket.h"

namespace psio
{
	class SSLSocket : public StreamSocket
	{
	public:

		SSLSocket(Network* net, std::unique_ptr<TCPSocket> sock, SSL_CTX* ctx) :
			m_net(net),
			m_sock(std::move(sock)),
			m_ctx(ctx),
			m_ssl(nullptr),
			m_connect_complete_handler(nullptr),
			m_state(HandshakeState::CONNECT_STATE_UNSET),
			m_next_state(HandshakeState::CONNECT_STATE_UNSET)
		{}
		~SSLSocket() override = default;
		
		int SSLAccept(Socket::CompletionHandler handler);
		int Accept(ConnectHandler handler);

		// create a context for a client
		SSL_CTX* createClientContext();

		// stream socket implementations
		int Connect(SocketAddress& addr, bool blocking, Socket::CompletionHandler handler);

		//  socket implementations
		bool Close();
		int Write(IOBuffer* buf, Socket::CompletionHandler handler);
		int Read(IOBuffer* buf, Socket::CompletionHandler handler);
		SocketAddress* getRemoteAddress();

	protected:
		Network* m_net;
		SSL_CTX* m_ctx;
		SSL* m_ssl;
		std::unique_ptr <TCPSocket> m_sock;
		SocketAddress m_addr;

		Socket::CompletionHandler m_connect_complete_handler;

		// handshake state machine

		enum class HandshakeState {
			CONNECT_STATE_UNSET,
			CONNECT_STATE_TRANSPORT_CONNECT,
			CONNECT_STATE_TRANSPORT_COMPLETED,
			CONNECT_STATE_SSL_CONNECT,
			CONNECT_STATE_SSL_CONNECTED,
			CONNECT_STATE_DONE
		};
		HandshakeState m_state;
		HandshakeState m_next_state;	

		void OnHandshakeIOComplete(int result);
		int DoHandshakeLoop(int res);
		int DoTransportCompleted(int result);
		int DoConnectSSL();
		int DoSSLConnected(int result);
	};
}

#endif // !PSIO_NET_SSLSOCKET_H
