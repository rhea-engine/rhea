#include <stdio.h>

#include "Connection.h"
#include "Request.h"
#include "Response.h"
#include "Http.h"

#define IO_BUFLEN 8192

namespace Hermes
{
	Connection::Connection(Hermes::TcpStream* conn, IRequestHandlerFactory* req_handler_factory) :
		m_conn(conn),
		m_req_handler_factory(req_handler_factory),
		m_status(STATE_NONE),
		writer(0),
		reader(0)
	{
		m_port = 0;
		if (conn) {
			sprintf_s(m_addr, INET6_ADDRSTRLEN, "%s", conn->get_remote_ip()->get_s(m_addr));
			m_port = conn->get_port();
		}
		else {
			sprintf_s(m_addr, INET6_ADDRSTRLEN, "NOT CONNECTED");			
		}
		// start with http protocol
		m_protocol = new Http();
		m_protocol->onConnection(this);
	}

	Connection::~Connection()
	{
		if (outbuf)
			free(outbuf);
		if (m_protocol)
			delete m_protocol;
		// delete the corresponding tcp stream. the stream is never used anywhere else.
		if (m_conn)
			delete m_conn;
	}

	void Connection::write(unsigned char* buf, size_t len)
	{
		if (!outbuflen)
		{
			outbuf = (unsigned char*)malloc(IO_BUFLEN);
			if (!outbuf) {
				printf("Could not allocate outbuf");
				return;
			}
			outbuflen = IO_BUFLEN;
			memcpy(outbuf, buf, len);
			outlen = len;
		}
		else {
			if (outlen + len > outbuflen)
			{
				unsigned char* new_outbuf = (unsigned char*)realloc(outbuf, outlen + len);
				if (!new_outbuf) {
					printf("Could not allocate outbuf");
					return;
				}
				outbuf = new_outbuf;
				outbuflen = outlen + len;
			}
			memcpy(outbuf + outlen, buf, len);
			outlen += len;
		}
	}

	void Connection::close()
	{
		m_status = STATE_CLOSING;
	}
	
	Connection::CONN_STATE Connection::iterate()
	{
		int received = 0;
		unsigned char buf[IO_BUFLEN];
		static unsigned char* msg;
		static int buflen = 0;
		static int len = 0;

		if (m_status == STATE_CLOSED)
			return m_status;

		if (m_conn->inputPending())
		{
			int ret = m_conn->read(buf, IO_BUFLEN, received);

			if (ret > 0)
			{
				// if data is available let protocol handle it
				if (received)
					m_protocol->onData(buf, received);



				// some telnet clients send null chars - replace them
				/*for (int i = 0; i < received; i++)
				{
					if (buf[i] == 0x00)
						buf[i] = 0x0A; // New Line
				}
				*/
			}
			else if (!ret)
			{
				// graceful exit
				m_status = STATE_CLOSING;
			}
		}
			

		// send outbuf if any
		if (outlen - writer > 0)
		{
			int sent;
			int ret_sent = m_conn->write((uint8_t*)(outbuf + writer), (int)outlen, sent);
			if (ret_sent > 0) {
				writer += sent;
				if (writer >= outlen)
				{
					writer = 0;
					outlen = 0;
				}
			}
			else if (!ret_sent)
			{
				// graceful exit
				m_status = STATE_CLOSING;
				writer = 0;
				outlen = 0;
			}
		}

		// If closing: close the connection if output buffer is empty
		if (!(outlen - writer) && m_status == STATE_CLOSING) {
			m_conn->close();
			delete m_conn;
			m_conn = NULL;
			m_status = STATE_CLOSED;
		}

		return m_status;
	}
}