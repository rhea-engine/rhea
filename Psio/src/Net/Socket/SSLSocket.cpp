/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Psio/Log.h>

#include <Psio/Net/Socket/SSLSocket.h>

namespace psio
{

	int SSLSocket::SSLAccept(Socket::CompletionHandler handler)
	{
		// initialize ssl context
		SOCKET client_fd = m_sock->GetRawSocket();
		m_ssl = SSL_new(m_ctx);
		SSL_set_fd(m_ssl, (int)client_fd);

		int ret = SSL_accept(m_ssl);
		if (ret <= 0) {
			int ret_ssl = SSL_get_error(m_ssl, ret);
			if (ret_ssl == SSL_ERROR_WANT_READ || ret_ssl == SSL_ERROR_WANT_WRITE) {
				int flag = 0;
				if (ret_ssl == SSL_ERROR_WANT_READ)
					flag = Network::IOFLAG::READ;
				if (ret_ssl == SSL_ERROR_WANT_WRITE)
					flag = Network::IOFLAG::WRITE;
				m_net->Watch(client_fd, flag, false, [&, handler, client_fd](int result)->void {
					int ret = SSL_accept(m_ssl);
					if (ret <= 0) {
						int ret_ssl = SSL_get_error(m_ssl, ret);
						if (!(ret_ssl == SSL_ERROR_WANT_READ || ret_ssl == SSL_ERROR_WANT_WRITE)) {
							// an error in ssl accept occured
							ERR_print_errors_fp(stderr);
							int syserr = GetLastError();
							DLOG(LOG_ERROR) << "SSLAccept failed: " << ret_ssl << " syserr=" << syserr;

							m_net->UnWatch(client_fd);
							handler(-1);
						}
						// else try again
					}
					else {
						// accept successful
						m_net->UnWatch(client_fd);
						handler(0);
					}
					});
			}
			return ret_ssl;
		}
		else {
			// accept did not block
			handler(0);
		}

		return 0;
	}

	int SSLSocket::Accept(ConnectHandler handler)
	{
		// TODO
		DLOG(LOG_ERROR) << "SSLSocket::Accept not implemented";
		abort();
		return 0;
	}
	
	SSL_CTX* SSLSocket::createClientContext()
	{
		const SSL_METHOD* method;
		SSL_CTX* ctx;

		method = TLS_client_method();

		ctx = SSL_CTX_new(method);
		if (!ctx) {
			LOG(LOG_ERROR) << "Unable to create SSL context";
			ERR_print_errors_fp(stderr);
			return nullptr;
		}

		return ctx;
	}

	int SSLSocket::Connect(SocketAddress& addr, bool blocking, Socket::CompletionHandler handler)
	{
		// initialize client ssl context
		m_ctx = createClientContext();
		if (m_ctx == nullptr)
			return -1;

		m_addr = addr;
		m_sock = std::move(std::unique_ptr<TCPSocket>(m_net->CreateTCPSocket()));
		m_next_state = HandshakeState::CONNECT_STATE_TRANSPORT_COMPLETED;
		m_connect_complete_handler = handler;
	
		int ret = m_sock->Connect(addr, blocking, [&](int ret) {
			return DoHandshakeLoop(ret);
			});
		if (ret == Network::Err::IO_PENDING)
			return ret;
		if (ret == 0)
			return DoHandshakeLoop(ret);
		return 0;
	}

	void SSLSocket::OnHandshakeIOComplete(int result) {
		m_net->UnWatch(m_sock->GetRawSocket());
		int res = DoHandshakeLoop(result);
		if (res != Network::Err::IO_PENDING) {
			// call completion handler
			m_connect_complete_handler(res);
		}
	}

	int SSLSocket::DoHandshakeLoop(int res) 
	{
		do
		{
			m_state = m_next_state;

			switch (m_state) {
			case HandshakeState::CONNECT_STATE_TRANSPORT_COMPLETED:
				res = DoTransportCompleted(res);
				break;
			case HandshakeState::CONNECT_STATE_SSL_CONNECT:
				// do ssl connect			
				res = DoConnectSSL();
				break;
			case HandshakeState::CONNECT_STATE_SSL_CONNECTED:
				res = DoSSLConnected(res);
				break;
			case HandshakeState::CONNECT_STATE_DONE:
				break;
			default:
				LOG(LOG_DEBUG) << "SSLSocket::DoHandshakeLoop: State not implemented: " << (int)m_state;
				m_state = HandshakeState::CONNECT_STATE_UNSET;
			}
		} while ((res != Network::Err::IO_PENDING) && (m_state != HandshakeState::CONNECT_STATE_UNSET && m_state != HandshakeState::CONNECT_STATE_DONE));
	
		return res;
	}

	int SSLSocket::DoTransportCompleted(int result)
	{
		if (result >= 0) {
			m_next_state = HandshakeState::CONNECT_STATE_SSL_CONNECT;
			m_ssl = SSL_new(m_ctx);
			SSL_set_fd(m_ssl, (int)m_sock->GetRawSocket());
			
			// set verification options

			// set hostname for SNI
			SSL_set_tlsext_host_name(m_ssl, m_addr.GetHost().c_str());

			std::string cert_path = m_net->GetSetting("ssl_root_ca");
			if (!SSL_CTX_load_verify_locations(m_ctx, cert_path.c_str(), nullptr)) {
				LOG(LOG_ERROR) << "could not load cacert " << cert_path;
			}
			SSL_CTX_set_verify(m_ctx, SSL_VERIFY_PEER, nullptr);
			SSL_CTX_set_verify_depth(m_ctx, 4);
			return 0;
		}
		else {
			m_next_state = HandshakeState::CONNECT_STATE_UNSET;
			m_connect_complete_handler(result);
			// handler could destroy *this
			return Network::Err::IO_PENDING;
		}
		return result;
	}

	int SSLSocket::DoConnectSSL()
	{
		/* initiate the TLS/SSL handshake with an TLS/SSL server */

		int ret = SSL_connect(m_ssl);
		if (ret <= 0) {
			m_next_state = m_next_state = HandshakeState::CONNECT_STATE_SSL_CONNECT;
			int ret_ssl = SSL_get_error(m_ssl, ret);
			if (ret_ssl == SSL_ERROR_WANT_READ || ret_ssl == SSL_ERROR_WANT_WRITE) {
				int flag = 0;
				if (ret_ssl == SSL_ERROR_WANT_READ)
					flag = Network::IOFLAG::READ;
				if (ret_ssl == SSL_ERROR_WANT_WRITE)
					flag = Network::IOFLAG::WRITE;
				m_net->Watch(m_sock->GetRawSocket(), flag, false, [&](int result)->void {
					OnHandshakeIOComplete(result);
					});
				return Network::Err::IO_PENDING;
			}
			else {				
				m_connect_complete_handler(-2);
				// handler could destroy *this
				return Network::Err::IO_PENDING;
			}
		}
		else {
			// non-blocking result
			ret = SSL_get_verify_result(m_ssl);
			if (!(ret == X509_V_OK)) {
				DLOG(LOG_DEBUG) << "verfication failed: " << ret;
			}
			else {
				DLOG(LOG_DEBUG) << "verfication succeeded";
			}
			m_next_state = m_next_state = HandshakeState::CONNECT_STATE_SSL_CONNECTED;
			return 0;
		}
	}

	int SSLSocket::DoSSLConnected(int result)
	{
		DLOG(LOG_DEBUG) << "SSL connected";
		if (result >= 0)
			m_next_state = HandshakeState::CONNECT_STATE_DONE;
		return result;
	}

	bool SSLSocket::Close()
	{
		SSL_shutdown(m_ssl);
		SSL_free(m_ssl);
		bool ret = false;
		if (m_sock) {
			 ret = m_sock->Close();		
		}
		return ret;
	}

	int SSLSocket::Write(IOBuffer* buf, Socket::CompletionHandler handler)
	{
		SOCKET client_fd = m_sock->GetRawSocket();

		int ret = SSL_write(m_ssl, buf->reader(), (int)buf->Remaining());
		if (ret <= 0) {
			int ret_ssl = SSL_get_error(m_ssl, ret);
			if (ret_ssl == SSL_ERROR_WANT_READ || ret_ssl == SSL_ERROR_WANT_WRITE) {
				int flag = 0;
				if (ret_ssl == SSL_ERROR_WANT_READ)
					flag = Network::IOFLAG::READ;
				if (ret_ssl == SSL_ERROR_WANT_WRITE)
					flag = Network::IOFLAG::WRITE;
				m_net->Watch(client_fd, flag, false, [&, client_fd, buf, handler](int result)->void {
					int ret = SSL_write(m_ssl, buf->reader(), (int)buf->Remaining());
					if (ret <= 0) {
						int ret_ssl = SSL_get_error(m_ssl, ret);
						if (!(ret_ssl == SSL_ERROR_WANT_READ || ret_ssl == SSL_ERROR_WANT_WRITE)) {
							// an error in ssl write occured
							ERR_print_errors_fp(stderr);
							int syserr = GetLastError();
							DLOG(LOG_ERROR) << "SSLwrite failed: " << ret_ssl << " syserr=" << syserr;
							m_net->UnWatch(client_fd);
							handler(-1);
						}
						// else try again
					}
					else {
						buf->Consume(ret);
						DLOG(LOG_DEBUG) << "bytes written: " << ret;
						// write successful
						m_net->UnWatch(client_fd);
						handler(0);
					}
					});
				return Network::Err::IO_PENDING;
			}
			else {
				DLOG(LOG_DEBUG) << "SSL write failed(1): " << ret_ssl;
				return -2;
			}
		}
		else {
			// write did not block
			buf->Consume(ret);
			DLOG(LOG_DEBUG) << "bytes written(1): " << ret;
			return ret;
		}
	}

	int SSLSocket::Read(IOBuffer* buf, Socket::CompletionHandler handler)
	{
		SOCKET client_fd = m_sock->GetRawSocket();

		if (buf->FreeBufSize() < 8 * 1024)
			buf->Grow(64 * 1024);
		int ret = SSL_read(m_ssl, buf->writer(), (int)buf->FreeBufSize());
		if (ret <= 0) {
			int ret_ssl = SSL_get_error(m_ssl, ret);
			if (ret_ssl == SSL_ERROR_WANT_READ || ret_ssl == SSL_ERROR_WANT_WRITE) {
				int flag = 0;
				if (ret_ssl == SSL_ERROR_WANT_READ)
					flag = Network::IOFLAG::READ;
				if (ret_ssl == SSL_ERROR_WANT_WRITE)
					flag = Network::IOFLAG::WRITE; 
				m_net->Watch(client_fd, flag, false, [&, client_fd, buf, handler](int result)->void {
					if (buf->FreeBufSize() < 8 * 1024)
						buf->Grow(64 * 1024);
					int ret = SSL_read(m_ssl, buf->writer(), (int)buf->FreeBufSize());
					if (ret <= 0) {
						int ret_ssl = SSL_get_error(m_ssl, ret);
						if (!(ret_ssl == SSL_ERROR_WANT_READ || ret_ssl == SSL_ERROR_WANT_WRITE)) {
							// an error in ssl write occured
							ERR_print_errors_fp(stderr);
							int syserr = GetLastError();
							DLOG(LOG_ERROR) << "SSLread failed: " << ret_ssl << " syserr=" << syserr;
							m_net->UnWatch(client_fd);
							handler(-1);
						}
						// else try again
					}
					else {
						DLOG(LOG_DEBUG) << "ssl: read " << ret << " bytes";
						// read successful
						buf->Produce(ret);
						m_net->UnWatch(client_fd);
						handler(ret);
					}
					});
				return Network::Err::IO_PENDING;
			}
			else {
				DLOG(LOG_DEBUG) << "SSL read failed(1): " << ret_ssl;
				return  Network::Err::FAILED;
			}
		}
		else {
			// read did not block
			DLOG(LOG_DEBUG) << "ssl: read (1) " << ret << " bytes";
			buf->Produce(ret);
			return ret;
		}

		return 0;
	}
	SocketAddress* psio::SSLSocket::getRemoteAddress()
	{
		return m_sock->getRemoteAddress();
	}
}