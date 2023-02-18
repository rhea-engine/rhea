#include <string>

#include "Http.h"
//#include "Request.h"
#include "HttpStream.h"
#include "HttpResponse.h"
//#include "Connection.h"
#include <Psio/Net/IOBuffer.h>
#include <Psio/Log.h>

#define MSG_BUFLEN 8192

using std::string;

namespace Psio
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

	Http::~Http()
	{
		if (msg)
			free(msg);
		if (echo_buf)
			free(echo_buf);
	}

	int Http::parseRequestLine(unsigned char* buf, unsigned char* end_p, HttpHeaders* headers)
	{
		PARSE_STATE parse_state = BEGIN_LINE;
		unsigned char* token = buf;
		unsigned char* bufstart = buf;
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
						return 0;
					
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
				if ((*buf == ' ') || (*buf == '\r') || (*buf == '\n')){
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
			string mth = string((char*)method_s, method_end_s - method_s);

			string val;
			if (uri)
				val = string((char*)uri, uri_end - uri);

			headers->SetMethod(mth);
			headers->SetUrl(val);
			return (int)(buf - bufstart);
		}
		else
			return 0;
	}

	void Http::OnData(Network::CompletionHandler data_handler)
	{
		m_ondata_handler = data_handler;
	}

	int Http::OnRequest(std::unique_ptr<Psio::StreamSocket> new_sock, RequestHandler onrequest_handler)
	{
		m_sock = std::move(new_sock);
		m_next_state = HTTP_TERMINALCHECK;
		m_echo = false;
		m_onrequest_handler = onrequest_handler;

		m_inbuf = std::make_unique<Psio::IOBuffer>(64 * 1024);
		m_outbuf = std::make_unique<Psio::IOBuffer>(64 * 1024);

		// start the http server state machine
		return DoLoop(0);
	}

	void Http::Write(const char* buf, size_t buflen, bool send)
	{
		m_outbuf->Write(buf, buflen);
		// only send if no buffering requested and no send callback is pending
		if (send && m_send_state == HTTP_SEND_PENDING)
			DoWrite(0);
	}

	int Http::End()
	{
		// flush all remaining data to recipient
		m_res_end = true;
		return DoSendLoop(m_outbuf->Remaining());
	}

	int Http::DoEnd(int result)
	{
		// prepare state machine for next request
		m_send_state = HTTP_SEND_FINISHED;
		m_next_send_state = HTTP_SEND_FINISHED;
		m_state = HTTP_NONE;
		m_response.reset();
		m_request.reset();

		return result;
	}

	void Http::CallSendLoop(int result)
	{
		int res = DoSendLoop(result);
		if ((res != Network::Err::IO_PENDING) && (m_res_end)) {
			delete this;
		}
	}

	int Http::DoSendLoop(int result)
	{
		do
		{
			m_send_state = m_next_send_state;
			m_next_send_state = HTTP_SEND_PENDING;

			switch (m_send_state) {
			case HTTP_SEND:
				result = DoWrite(result);
				break;
			case HTTP_SEND_COMPLETED:
				result = DoWriteCompleted(result);
				break;
			case HTTP_SEND_FINISHED:
				result = DoEnd(result);
			}
		} while (result != Network::Err::IO_PENDING &&
			m_send_state != HTTP_SEND_FINISHED && m_send_state != HTTP_SEND_PENDING);
		return result;
	}

	int Http::DoWrite(int result)
	{
		if (!m_outbuf->Remaining())
			return Network::Err::IO_PENDING;

		m_next_send_state = HTTP_SEND_COMPLETED;
		return m_sock->Write(m_outbuf.get(), [&](int result) {
			CallSendLoop(result);			
			});
	}

	int Http::DoWriteCompleted(int result)
	{
		m_next_send_state = HTTP_SEND_FINISHED;
		if (result < 0) {
			LOG(LOG_ERROR) << "Error sending response: " << result;
			return result;
		}
		if (m_outbuf->Remaining() > 0) {
			// still data to send - restart sending
			m_next_send_state = HTTP_SEND;
			return 0;
		}
		// no more data
		if (!m_res_end) {
			m_next_send_state = HTTP_SEND_PENDING;
			return Network::Err::IO_PENDING;
		}
		else {
			m_next_send_state = HTTP_SEND_FINISHED;
			return result;
		}
	}

	void Http::echo(unsigned char* buf, size_t len)
	{
		// dont echo NVT commands
		if (buf[0] == 0xff)
			return;

		// m_connection->write(buf, len);
		//TODO m_sock->Write(buf, nullptr); // TODO
	}

	int Http::DoLoop(int result)
	{
		do
		{
			m_state = m_next_state;
			m_next_state = HTTP_NONE;

			switch (m_state) {
			case HTTP_TERMINALCHECK:
				result = DoTerminalCheck(result);
				break;
			case HTTP_TERMINALCHECK_COMPLETED:
				result = DoTerminalCheckCompleted(result);
				break;
			case HTTP_REQUEST:
				result = DoRequestLine(result);
				break;
			case HTTP_REQUEST_COMPLETED:
				result = DoRequestLineCompleted(result);
				break;
			case HTTP_HEADER:
				result = DoHeaders(result);
				break;
			case HTTP_HEADER_COMPLETED:
				result = DoHeadersCompleted(result);
				break;
			case HTTP_BODY:
				break;
			}

		} while (result != Network::IO_PENDING && m_state != HTTP_NONE);

		return result;
	}

	void Http::CallDoLoop(int result)
	{
		int res = DoLoop(result);
		if ((res != Network::Err::IO_PENDING) && (m_ondata_handler))
			m_ondata_handler(res);
	}

	int Http::DoTerminalCheck(int result)
	{
		m_next_state = HTTP_TERMINALCHECK_COMPLETED;
		int err = m_sock->Read(m_inbuf.get(), [&](int err) {
			CallDoLoop(err);
			});
		return err;
	}

	int Http::DoTerminalCheckCompleted(int result)
	{
		// strip NVT for terminal connections
		while ((m_inbuf->Remaining() >= 1) && ((*m_inbuf)[0] == 0xFF)) {
			if (m_inbuf->Remaining() > 3) {
				if ((*m_inbuf)[1] == 0xFC && (*m_inbuf)[2] == 0x01) {
					
					// send telnet initial options

					echo_len = 0;

					// Set NVT mode to say that I will echo back characters
					(*m_outbuf)[echo_len++] = (char)0xff;
					(*m_outbuf)[echo_len++] = (char)0xfb;
					(*m_outbuf)[echo_len++] = (char)0x01;

					// Set NVT requesting that the remote system not/dont echo back characters
					(*m_outbuf)[echo_len++] = (char)0xff;
					(*m_outbuf)[echo_len++] = (char)0xfe;
					(*m_outbuf)[echo_len++] = (char)0x01;

					// Set NVT mode to say that I will supress go-ahead. Stops remote clients from doing local linemode
					(*m_outbuf)[echo_len++] = (char)0xff;
					(*m_outbuf)[echo_len++] = (char)0xfb;
					(*m_outbuf)[echo_len++] = (char)0x03;
					m_echo = true;

					m_sock->Write(m_outbuf.get(), nullptr);
				}
				m_inbuf->Consume(3);
			}
			else {
				m_next_state = HTTP_TERMINALCHECK;
				return Network::Err::IO_PENDING;
			}
		}

		m_next_state = HTTP_REQUEST_COMPLETED;
		if (m_inbuf->Remaining())
			return (int)m_inbuf->Remaining();

		return DoRequestLine(0);
	}

	int Http::DoRequestLine(int result)
	{
		m_next_state = HTTP_REQUEST_COMPLETED;
		int err = m_sock->Read(m_inbuf.get(), [&](int err) {
			CallDoLoop(err);
			});
		return err;
	}

	int Http::DoRequestLineCompleted(int result) {

		if (result < 0) {
			m_next_state = HTTP_NONE;
			return result;
		}

		if (m_echo) {
		//	(*m_outbuf)[0] = (*m_inbuf)[0];
		//	m_outbuf->Produce(1);
		}

		// search for newline
		char* buf = m_inbuf->reader();
		size_t buf_len = m_inbuf->Remaining();
		for (int i = 0; i < buf_len; i++) {
			if (buf[i] == 0x0A)
			{
				m_headers = new HttpHeaders();
				int offset = parseRequestLine((unsigned char*)m_inbuf->reader(), (unsigned char*)m_inbuf->reader() + i, m_headers);
				if (offset) {
					m_inbuf->Consume(i);
					m_next_state = m_inbuf->Remaining() > 0 ? HTTP_HEADER_COMPLETED : HTTP_HEADER;
					return (int)m_inbuf->Remaining();
				}
				else {
					m_next_state = HTTP_NONE;
					m_response->sendError(400);
					return -400;
				}
			}
		}
		// no newline found - need more data
		m_next_state = HTTP_REQUEST;
		return 0;
	}

	int Http::DoHeaders(int result)
	{
		m_next_state = HTTP_HEADER_COMPLETED;
		int err = m_sock->Read(m_inbuf.get(), [&](int err) {
			CallDoLoop(err);
			});
		return err;
	}

	int Http::DoHeadersCompleted(int result)
	{
		size_t end_offset = std::string::npos;

		m_next_state = HTTP_BODY;
		if (result < 0) {
			return result;
		}

		// search for end of headers
		end_offset = HttpStream::LocateEndOfHeadersHelper(m_inbuf->reader(), m_inbuf->Remaining(), 0, false);

		// end of headers not found - too few data
		if (end_offset == std::string::npos) {
			m_next_state = HTTP_HEADER;
			// safety check for too large headers
			if (end_offset > 256 * 1024)
				return Network::Err::RESPONSE_HEADERS_TOO_BIG;
			return Network::Err::IO_PENDING;
		}

		// parse headers		
		HttpHeaders::ParseHeaders(std::string(m_inbuf->reader(), end_offset), 0, m_headers);
		m_inbuf->Consume(end_offset);

		// create request object
		m_request = std::make_unique<HttpRequest>(this);
		m_request->SetHeaders(m_headers);

		// create Response object
		m_response = std::make_unique<HttpResponse>(this);
		m_send_state = HTTP_SEND_PENDING;

		// call handler for onRequest
		// this may destroy |this|
		int ret = m_onrequest_handler(m_request.get(), m_response.get());

		if (ret != Network::Err::IO_PENDING) {
			delete this;
			return Network::Err::IO_PENDING;			
		}

		if (m_has_body) {
			m_next_state = HTTP_BODY;
		}
		else {
			m_next_state = HTTP_NONE;
		}
		if (m_inbuf->Remaining() > 0)
			return (int)m_inbuf->Remaining();
		else
			return Network::Err::IO_PENDING;
	}

	int Http::DoResponse(int result)
	{
		return 0;
	}
}