#ifndef CONNECTION_H
#define CONNECTION_H

#include <list>
#include <memory>
#include <cstdint>

#include "TcpStream.h"
#include "IProtocol.h"
#include "IRequestHandlerFactory.h"

namespace Hermes
{
	class Request;
	class Response;	

	class Connection
	{
	public:
		enum CONN_STATE {
			STATE_NONE,
			STATE_MSG_HEADER,
			STATE_MSG_BODY,
			STATE_CLOSING,
			STATE_CLOSED
		};

		Connection(Hermes::TcpStream* conn, IRequestHandlerFactory* req_handler_factory);
		virtual ~Connection();

		// return the TcpStram object
		Hermes::TcpStream* get_connection() { return m_conn; }
		// poll connection and read and write in a non-blocking manner returning the current connection state
		CONN_STATE iterate();
		// write buf to output buffer
		virtual void write(unsigned char* buf, size_t len);
		// close the connection - sends all remaining outbuf
		virtual void close();
		// get conn info as a string
		char* get_remote_s() { return m_addr; }
		// get port number
		uint16_t get_port() { return m_port; }
		// set a request handler
		void setRequestHandler(IRequestHandlerFactory* request_handler_factory) {
			m_req_handler_factory = request_handler_factory;
		}
		// get the request handler 
		IRequestHandlerFactory* getRequestHandler() { return m_req_handler_factory; }

	protected:
		Hermes::TcpStream* m_conn;
		IRequestHandlerFactory* m_req_handler_factory;
		CONN_STATE m_status;
		unsigned char* outbuf = NULL;
		size_t outlen = 0;
		size_t outbuflen = 0;
		size_t writer = 0;
		size_t reader;
		char m_addr[INET6_ADDRSTRLEN];
		IProtocol* m_protocol = 0;
		bool m_echo = false;
		uint16_t m_port;
	};
}

#endif // !CONNECTION_H
