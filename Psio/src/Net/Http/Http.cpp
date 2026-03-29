#include <string>

#include <Psio/Net/Http/Http.h>
#include <Psio/Net/Http/HttpStream.h>
#include <Psio/Net/Http/HttpResponse.h>
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

	int Http::Write(const char* buf, size_t buflen, bool no_buffering)
	{
		if (m_res_end)
			return -1;

		m_outbuf->Write(buf, buflen);
		if (!no_buffering)
			return -1;
		
		// call send loop
		return DoSendLoop((int)m_outbuf->Remaining());
	}

	int Http::ResponseEnd()
	{
		if (!m_res_end) {
			// flush all remaining data to recipient
			m_res_end = true;
			return DoSendLoop((int)m_outbuf->Remaining());
		}
		else
		{
			return -1;
		}
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
			m_next_send_state = HTTP_SEND_NONE;

			//LOG(LOG_DEBUG) << "DoSendLoop(" << result << ") - state: " << m_send_state;

			switch (m_send_state) {
			case HTTP_SEND_PENDING:
				result = Network::Err::IO_PENDING;
				m_next_send_state = HTTP_SEND_PENDING;
				break;
			case HTTP_SEND:
				result = DoWrite(result);
				break;
			case HTTP_SEND_COMPLETED:
				result = DoWriteCompleted(result);
				break;
			case HTTP_SEND_FINISHED:
				result = DoEnd(result);
			}
		} while (result != Network::Err::IO_PENDING && m_send_state != HTTP_SEND_NONE);
		return result;
	}

	int Http::DoWrite(int result)
	{
		if (!result) {
			m_next_send_state = HTTP_SEND_COMPLETED;
			return result;
		}
		if (!m_outbuf->Remaining()) {
			m_next_send_state = HTTP_SEND;
			return Network::Err::IO_PENDING;
		}

		m_next_send_state = HTTP_SEND_COMPLETED;
		return m_sock->Write(m_outbuf.get(), [&](int result) {
			CallSendLoop(result);			
			});
	}

	int Http::DoWriteCompleted(int result)
	{
		m_next_send_state = HTTP_SEND_FINISHED;
		if (result < 0) {
			if (result != Network::Err::IO_PENDING) {
				LOG(LOG_ERROR) << "Error sending response: " << result;
			}
			else {
				m_next_send_state = HTTP_SEND;
			}
			return result;
		}
		if (m_outbuf->Remaining() > 0) {
			// still data to send - restart sending
			m_next_send_state = HTTP_SEND;
			return (int)m_outbuf->Remaining();
		}
		// no more data
		if (!m_res_end) {
			m_next_send_state = HTTP_SEND;
			return Network::Err::IO_PENDING;
		}
		else {
			m_next_send_state = HTTP_SEND_FINISHED;
			return result;
		}
	}

	int Http::DoEnd(int result)
	{
		// prepare state machine for next request
		m_send_state = HTTP_SEND_FINISHED;
		m_next_send_state = HTTP_SEND_NONE;
		m_state = HTTP_NONE;

		// log the response
		LOG(LOG_INFO) << m_request->headers.GetMethod() << " \"" << m_request->headers.GetUrl() << "\" " << m_response->getResponseCode() << " " << m_response->getContentLength();

		// Note: precondition is that request and response are handled entirely
		m_response.reset();
		m_request.reset();

		// close the connection

		// keep-alive?
		bool keep_alive = false;
		if (!keep_alive) {

			if (m_onclose_handler)
				m_onclose_handler(0);
			return Network::Err::IO_PENDING;
		}

		return result;
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

			// LOG(LOG_DEBUG) << "DoLoop(" << result << ") - state: " << m_state;

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
				result = DoBody(result);
				break;			
			case HTTP_BODY_COMPLETED:
				result = DoBodyCompleted(result);
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
		m_next_send_state = HTTP_SEND_PENDING;

#ifdef _DEBUG_HTTP
		std::vector<std::string> headers;
		headers = m_headers->GetHeaderList();
		for (auto it = headers.begin(); it != headers.end(); ++it)
			LOG(LOG_DEBUG) << "["  << *it << "] " << (*m_headers)[(*it)];
#endif

		// call handler for onRequest
		// the handler should setup handlers for request and response
		int ret = m_onrequest_handler(m_request.get(), m_response.get());

		// determine if we have a body
		// only a body if we have content-length
		if (m_headers->HasHeader("content-length")) {
			m_has_body = true;			
			m_body_len = std::stoi((*m_headers)["content-length"]);
			m_body_read = 0;
			m_next_state = HTTP_BODY;
		}
		else {
			m_has_body = false;
			m_next_state = HTTP_BODY_COMPLETED;
		}


		if (m_inbuf->Remaining() > 0) {
			if (!m_has_body)
				LOG(LOG_ERROR) << "No Request Body but still bytes remaining: " << m_inbuf->Remaining();
			return (int)m_inbuf->Remaining();
		}
		else
			return 0;
	}

	int Http::DoBody(int result)
	{
		char* chunk = m_inbuf->reader();
		size_t chunk_len = m_inbuf->Remaining();

		// if we have still data in the buffer handle it as chunk
		if (chunk_len > 0) {
			m_request->SendData(m_inbuf.get());
			// how many bytes where consumed by the onData handler?
			chunk_len -= m_inbuf->Remaining();
			m_body_read += chunk_len;
		}
		// already read the whole body?
		if (m_body_read + m_inbuf->Remaining() >= m_body_len) {
			m_next_state = HTTP_BODY_COMPLETED;
			return (int)m_inbuf->Remaining();
		}

		// wait for more data of body
		m_next_state = HTTP_BODY;
		int err = m_sock->Read(m_inbuf.get(), [&](int err) {
			CallDoLoop(err);
			});
		return err;
	}

	int Http::DoBodyCompleted(int result)
	{
		m_next_state = HTTP_NONE;

		// request finished - sending data is possible
		m_next_send_state = HTTP_SEND;

		// response already completly sent to send buffer?
		if (m_res_end) {
			DoSendLoop((int)m_outbuf->Remaining());
		}
		else {
			// send 'end' event	
			m_request->End(m_inbuf.get());
		}

		// exit the main loop
		return Network::Err::IO_PENDING;
	}

	int Http::DoResponse(int result)
	{
		return 0;
	}
}