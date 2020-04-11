#include <string.h>

#include "Http.h"
#include "Request.h"
#include "HttpResponse.h"
#include "Connection.h"

#define MSG_BUFLEN 8192

using std::string;

namespace Hermes
{
	enum http_header
	{
		HTTP_HEADER_ACCEPT
	};

	enum htttp_method
	{
		HTTP_UNKNOWN,
		HTTP_GET
	};

	enum PARSE_STATE
	{
		BEGIN_LINE,
		METHOD_VALUE_START,
		METHOD_VALUE,
		URI_VALUE_START,
		URI_VALUE,
		PROTO_VALUE_START,
		PROTO_VALUE
	};

	enum PARSE_HEADER_STATE
	{
		PHS_BEGIN_LINE,
		PHS_HEADER_START,
		PHS_HEADER,
		PHS_HEADER_VALUE_START,
		PHS_HEADER_VALUE,
		PHS_TRAILING
	};

	bool Http::parseRequestLine(unsigned char* buf, unsigned char* end_p)
	{
		PARSE_STATE parse_state = BEGIN_LINE;
		unsigned char* token = buf;
		size_t j = 0;
		htttp_method method = HTTP_UNKNOWN;
		unsigned char* uri = 0;
		unsigned char* uri_end = 0;
		unsigned char* method_s = 0;
		unsigned char* method_end_s = 0;

		size_t buflen;
		size_t unread_len;
		size_t tok_len;

		// skip leading whitespace
		while ((*buf == ' ') && (buf < end_p))
			buf++;

		// start with first token
		parse_state = METHOD_VALUE;
		token = buf;
		buflen = end_p - buf;
		unread_len = buflen;
		tok_len = 0;

		while (buf < end_p)
		{
			switch (parse_state) {
			case METHOD_VALUE:
				if (*buf == ' ')
				{
					method_s = token;
					method_end_s = buf;
					parse_state = URI_VALUE_START;
					token = ++buf;
				}
				else if (*buf == 0x0d)
				{
					// ignore empty line 
					if (token == buf)
						return true;
					
					method_s = token;
					method_end_s = buf;
					parse_state = PROTO_VALUE;
				}
				else
					buf++;
				break;
			case URI_VALUE_START:
				// skip whitespace
				if (*buf == ' ') {
					token++;
					buf++;
				}
				else
					parse_state = URI_VALUE;
				break;
			case URI_VALUE:
				if (*buf == ' ') {
					parse_state = PROTO_VALUE_START;
					uri = token;
					uri_end = buf;
					token = ++buf;
				}
				else
					buf++;
				break;

			case PROTO_VALUE_START:
				if (*buf == ' ') {
					token++;
					buf++;
				}
				else
					parse_state = PROTO_VALUE;
				break;
			case PROTO_VALUE:
				buf++;
				break;
			}
		}

		if (method_s) {
			string mth = string((char *)method_s, method_end_s - method_s);
	
			string val;
			if (uri)
				val = string((char *)uri, uri_end - uri);

			m_req->setCommand(mth, val);
			return true;
		}
		else
			return false;
	}

	bool Http::parseHeaderLine(unsigned char* buf, unsigned char* end_p)
	{
		PARSE_HEADER_STATE parse_state = PHS_BEGIN_LINE;
		unsigned char* token = buf;
		size_t j = 0;
		http_header hdr;
		htttp_method method = HTTP_UNKNOWN;
		unsigned char* value = 0;
		unsigned char* value_end = 0;
		unsigned char* header = 0;
		unsigned char* header_end = 0;

		size_t buflen;
		size_t unread_len;
		size_t tok_len;
		size_t val_len;

		// skip leading whitespace
		while ((*buf == ' ') && (buf < end_p))
			buf++;

		// start with first token
		parse_state = PHS_HEADER_START;
		token = buf;
		buflen = end_p - buf;
		unread_len = buflen;
		tok_len = 0;

		while (buf < end_p)
		{
			switch (parse_state) {
			case PHS_HEADER_START:
				if (*buf == ':') {
					tok_len = buf - token;
					header = token;
					header_end = buf;
					parse_state = PHS_HEADER_VALUE_START;
					*buf = '\0';

					// find value start and skip leading whitespace
					value = buf + 1;
					while (value < end_p && *value == ' ')
						value++;
					// if not empty chop trailing white space					
					while (end_p > value && (*end_p == ' ' || *end_p == 0x0d))
						end_p--;
					val_len = end_p - value;

					if (!strcmp((const char*)token, "Accept")) {
						hdr = HTTP_HEADER_ACCEPT;
						parse_state = PHS_HEADER_VALUE_START;
					}
					else
					{
						// unknown: ignore the header
						return true;
					}
					// line has been consumed - return
					return true;
				}
				else if (*buf == 0x0d)
				{
					// empty line
					return false;
				}
				else
					buf++;
				break;
			case PHS_HEADER_VALUE_START:
				// skip whitespace
				if (*buf == ' ') {
					buf++;
					break;
				}
				else {
					parse_state = PHS_HEADER_VALUE;
					token = buf;
				}
				break;
			case PHS_HEADER_VALUE:
				if (*buf == 0x0d) {
					// end of line
					value = token;
					value_end = buf;
					parse_state = PHS_TRAILING;
				}
				else
					buf++;
				break;
			case PHS_TRAILING:
				buf++;
				break;
			}
		}
		if (header && header_end && value && value_end)
		{
			// parsed without error
			if (!strncmp((const char*)header, "Accept", header_end - header)) {
				hdr = HTTP_HEADER_ACCEPT;
			}
		}

		return true;
	}

	unsigned char* Http::getLine(unsigned char* buf, size_t& reader, int& len)
	{
		// cleanup buffer and search for newline
		for (size_t i = reader; i < len; i++) {

			// strip NVT for terminal connections
			while (buf[i] == 0xFF)
			{
				if (i + 3 <= len) {
					memcpy(&buf[i], &buf[i + 3], len - i - 3);
					len -= 3;
					if (i >= len)
						break;
				}
			}
			// break from loop if after stripping the buffer is empty
			if (i >= len)
				break;

			// a newline ends a header line (not \n bit 0x0A because of network encoding)
			if (buf[i] == 0x0A)
			{

				//if(!parseHeaderLine(&buf[reader], &buf[i]))					
				//	m_state = HTTP_BODY;
				reader = i + 1;
				return &buf[i];

			}
		}
		return NULL;
	}

	void Http::onConnection(Connection* conn)
	{
		m_connection = conn;
	}

	Http::~Http()
	{				
		if (msg)
			free(msg);
		if (echo_buf)
			free(echo_buf);
	}

	void Http::echo(unsigned char* buf, size_t len)
	{
		// dont echo NVT commands
		if (buf[0] == 0xff)
			return;

		m_connection->write(buf, len);
	}

	void Http::onData(unsigned char* buf, size_t buflen)
	{
		unsigned char* eol = NULL;

		if (!msg)
		{
			msg = (unsigned char*)malloc(MSG_BUFLEN);
			if (!msg) {
				printf("Could not allocate msg memory\n");
				m_connection->close();
				return;
			}
			m_buflen = MSG_BUFLEN;
		}
		if (!echo_buf)
		{
			echo_buf = (unsigned char*)malloc(MSG_BUFLEN);
			if (!echo_buf) {
				printf("Could not allocate echo_buf memory\n");
				m_connection->close();
				return;
			}
			m_buflen = MSG_BUFLEN;
		}
		// enlarge io buffer if necessary
		if (m_buflen < m_len + buflen) {
			while (m_buflen < m_len + buflen)
				buflen += MSG_BUFLEN;
			unsigned char* newmsg = (unsigned char*)realloc(msg, m_buflen);
			if (newmsg)
				msg = newmsg;
			else {
				printf("Could not alloc buffer (%d bytes)\n", (int)m_buflen);
				m_connection->close();
				return;
			}
		}

		for (int i = 0; i < buflen; i++)
		{
			switch (m_state)
			{
			case HTTP_NONE:
				reader = msg;
				m_len = 0;
				m_state = HTTP_REQUEST;
				m_echo = false;
				m_req = std::make_shared<Request>(m_connection);
				m_res = std::make_shared<HttpResponse>(m_connection, m_req);
				m_has_body = false;
				
				// intentional drop through
			case HTTP_REQUEST:
				// strip NVT for terminal connections
				while (buf[i] == 0xFF)
				{
					if ((size_t)i + 3 <= buflen) {
						// check for echo command (IAC WILL ECHO)
						if (buf[i + 1] == 0xFC && buf[i + 2] == 0x01)
						{
							// send telnet initial options

							// Set NVT mode to say that I will echo back characters
							echo_buf[echo_len++] = 0xff;
							echo_buf[echo_len++] = 0xfb;
							echo_buf[echo_len++] = 0x01;

							// Set NVT requesting that the remote system not/dont echo back characters
							echo_buf[echo_len++] = 0xff;
							echo_buf[echo_len++] = 0xfe;
							echo_buf[echo_len++] = 0x01;



							// Set NVT mode to say that I will supress go-ahead. Stops remote clients from doing local linemode
							echo_buf[echo_len++] = 0xff;
							echo_buf[echo_len++] = 0xfb;
							echo_buf[echo_len++] = 0x03;
							m_echo = true;
						}
						i += 3;
					}
					else
						return; // wait for more input
				}
				// if the buffer is empty wait for data
				if (i == buflen)
					return;

				if (m_echo)
					echo_buf[echo_len++] = buf[i];

				// search for newline
				if (buf[i] == 0x0A)
				{
					// newline: end the msg line
					eol = &msg[m_len];
					if (parseRequestLine(reader, eol)) {
						m_state = HTTP_HEADER;
						reader = eol;
					}
					else {
						m_res->sendError(400);
					}
				}
				else {
					msg[m_len++] = buf[i];
				}
				break;
			case HTTP_HEADER:
				// strip NVT for terminal connections
				while (buf[i] == 0xFF)
				{
					if ((size_t)i + 3 <= buflen) {
						i += 3;
					}
					else
						return; // wait for more input
				}
				// if the buffer is empty wait for data
				if (i == buflen)
					return;

				if (m_echo)
					echo_buf[echo_len++] = buf[i];

				// search for newline
				if (buf[i] == 0x0A)
				{
					// newline: end the msg line
					eol = &msg[m_len];
					if (!parseHeaderLine(reader, eol)) {

						// header(s) finished - get corresponing handler
						reader = eol;
						IRequestHandler *handler = m_connection->getRequestHandler()->onRequest(m_req);
						
						// immediately send onRequest event
						if (handler) {
							m_req->setRequestHandler(handler);
							handler->onRequest(m_req, m_res);
						}

						// wait for stream or close it for body-less messages
						if (m_has_body) {
							m_state = HTTP_BODY;
							// todo if data in buffer send it
						}
						else {
							// request finished TODO: get this via event?
							m_state = HTTP_NONE;
							m_req->onEnd();
							m_req.reset();
							m_res.reset();
						}
					}					
					// consume the line
					reader = eol;					
				}
				else {
					msg[m_len++] = buf[i];
				}
				break;
			case HTTP_BODY:
				// body of message 

				// add buffer to command and handle it
				m_req->onData(buf, buflen);
				// todo: send onEnd if content-length reached

				break;
			}
		}

		if (m_echo && echo_len) {
			m_connection->write(echo_buf, echo_len);
			echo_len = 0;
		}

		return;
	}
}