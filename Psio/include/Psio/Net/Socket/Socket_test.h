#ifndef PSIO_NET_SOCKET_TEST_H
#define PSIO_NET_SOCKET_TEST_H

#include "../UrlRequest/UrlRequest.h"
#include <Psio/Psio.h>
#include "../Http/HttpTransaction.h"
#include "TCPSocket.h"
#include "../Http/HttpRequest.h"

namespace Psio {

	class MockTransaction : public HttpTransaction
	{
	public:
		int Start(HttpRequest& url, Network::CompletionHandler handler) {
			handler((int)m_len);
			return -1;
		}
		int ReadBody(IOBuffer* buf, size_t chunk_len, Network::CompletionHandler handler) {
			return -1;
		}

		void setdata(char* buf, size_t len) {
			m_buf = buf;
			m_len = len;
		}

		HttpHeaders* GetHeaders() { return nullptr; }

		char* m_buf = nullptr;
		size_t m_len = 0;
	};

	class FakeSocket : public TCPSocket
	{
	public:
		FakeSocket(Network* net) :
			TCPSocket(net)
		{
			m_buf = new IOBuffer(2048);
			m_sent_buf = new IOBuffer(2048);
		}
		~FakeSocket() {
			delete m_buf;
			delete m_sent_buf;
		}
		void SetData(char* buf, size_t buf_len) {
			m_buf->Write(buf, buf_len);
		}
		IOBuffer* GetSentData() {
			return m_sent_buf;
		}

		int Connect(SocketAddress& addr, bool blocking, Socket::CompletionHandler handler) override {
			handler(0);
			return -1;
		}

		int Write(IOBuffer* buf, Socket::CompletionHandler handler) override {
			m_sent_buf->Write(buf->reader(), buf->Remaining());
			m_wbuf_len = buf->Remaining();
			buf->Consume(buf->Remaining());
			w_handler = handler;
			if (!m_async) {
				handler((int)m_wbuf_len);
				return 0;
			} else
				return -1;
		}

		int Read(IOBuffer* buf, Socket::CompletionHandler handler) override
		{
			size_t len = m_buf->Remaining();
			buf->Write(m_buf->reader(), len);
			m_buf->Consume(len);
			m_rbuf_len += len;
			r_handler = handler;
			if (!m_async) {
				handler((int)len);
				return 0;
			}
			else if (!len)
				// eof
				return 0;
			else
				return -1;
		}

		void Poll(bool keep_alive) {
			if (m_wbuf_len > 0 && w_handler) {
				w_handler((int)m_wbuf_len);
				m_wbuf_len = 0;

			}
			if (r_handler) {
				if (m_rbuf_len > 0 || !keep_alive) {
					Socket::CompletionHandler handler = r_handler;
					int len = (int)m_rbuf_len;
					m_rbuf_len = 0;
					r_handler = NULL;
					handler(len);
				}
			}
		}

		IOBuffer* m_buf = nullptr;
		IOBuffer* m_sent_buf = nullptr;
		size_t m_wbuf_len = 0;
		size_t m_rbuf_len = 0;
		Socket::CompletionHandler w_handler = NULL;
		Socket::CompletionHandler r_handler = NULL;

		bool m_async = true;
	};

	class FakeNetwork : public Network
	{
	public:
		FakeNetwork(Engine* engine) :
			Network(engine)
		{}
		TCPSocket* CreateTCPSocket() override
		{
			return m_sock;
		}
		void SetSocket(TCPSocket* sock) {
			m_sock = sock;
		}
		class TCPSocket* m_sock = nullptr;
	};
}
#endif // !PSIO_NET_SOCKET_TEST_H