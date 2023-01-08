#include "HttpStream.h"
#include "../Network.h"
#include "../IOBuffer.h"
#include "../Socket/TCPSocket.h"
#include <Components/Psio/Util.h>
#include <Psio/Log.h>

namespace Psio
{
	HttpStream::~HttpStream()
	{
		if (m_req_buffer) {
			delete m_req_buffer;
			m_req_buffer = nullptr;
		}
		if (m_res_buffer) {
			delete m_res_buffer;
			m_res_buffer = nullptr;
		}		
		if (m_res_headers) {
			delete m_res_headers;
			m_res_headers = nullptr;
		}
	}

	int HttpStream::SendRequest(HttpRequest& request_info, Psio::Network::CompletionHandler handler)
	{
		m_request = request_info;
		std::string request;
		m_handler = handler;
		request = m_request.headers.GenerateRequestString(m_request.url.Path()) + m_request.headers.ToString();
	
		size_t request_headers_length = request.size();
		m_req_buffer = new IOBuffer(request_headers_length);
		memcpy(m_req_buffer->data(), request.data(), request_headers_length);
		m_req_buffer->Produce(request_headers_length);

		m_next_state = State::HTTPSTREAM_SEND_HEADERS;
		return DoLoop(0);
	}

	int HttpStream::ReadResponseHeaders(Network::CompletionHandler handler)
	{
		if (m_state == State::HTTPSTREAM_REQUEST_COMPLETED)
			return Network::Err::OK; 
		
		m_next_state = State::HTTPSTREAM_READ_HEADERS;
		m_handler = NULL; // TODO synchronous is not implemented yet

		int result = DoLoop(0);
		// not everything was read
		if (result == Network::Err::IO_PENDING)
			m_handler = handler;
		return result;
	}

	int HttpStream::ReadBody(IOBuffer* buf, size_t chunk_len, Psio::Network::CompletionHandler handler)
	{
		if (m_state == State::HTTPSTREAM_REQUEST_COMPLETED)
			return Network::Err::OK;

		m_next_state = State::HTTPSTREAM_READ_BODY;
		m_user_buffer = buf;
		m_user_buffer_len = chunk_len;
		m_handler = NULL;

		int result = DoLoop(0);
		// not everything was read
		if (result == Network::Err::IO_PENDING)
			m_handler = handler;
		return result;
	}

	int HttpStream::DrainBody(Psio::Network::CompletionHandler handler)
	{
		if (m_state == State::HTTPSTREAM_REQUEST_COMPLETED)
			return Network::Err::OK;

		m_next_state = State::HTTPSTREAM_DRAIN_BODY;	
		m_handler = NULL;

		int result = DoLoop(0);
		// not everything was read
		if (result == Network::Err::IO_PENDING)
			m_handler = handler;
		return result;
	}

	void HttpStream::CallDoLoop(int result)
	{
		int res = DoLoop(result);
		if ((res != Network::Err::IO_PENDING) && (m_handler != NULL))
			m_handler(res);
	}

	int HttpStream::DoLoop(int result)
	{
		int m_error = result;

		do
		{
			m_state = m_next_state;
			m_next_state = State::HTTPSTREAM_NONE;

			switch (m_state)
			{
			case State::HTTPSTREAM_NONE:
				break;
			case State::HTTPSTREAM_SEND_HEADERS:
				DLOG(LOG_DEBUG) << "HTTPSTREAM: Sending Headers";
				m_error = DoSendHeaders(m_error);
				break;
			case State::HTTPSTREAM_SEND_HEADERS_COMPLETED:
				DLOG(LOG_DEBUG) << "HTTPSTREAM: Sending Headers Completed";
				m_error = DoSendHeadersComplete(m_error);
				break;
			case State::HTTPSTREAM_SEND_BODY:
				m_error = DoSendBody(m_error);
				break;
			case State::HTTPSTREAM_SEND_BODY_COMPLETED:
				m_error = DoSendBodyComplete(m_error);
				break;
			case State::HTTPSTREAM_SEND_HEADERS_AND_BODY_COMPLETE:
				m_error = DoSendAllComplete(m_error);
				break;
			case State::HTTPSTREAM_READ_HEADERS:
				m_error = DoReadHeaders(m_error);
				break;
			case State::HTTPSTREAM_READ_HEADERS_COMPLETED:
				DLOG(LOG_DEBUG) << "HTTPSTREAM: Reading Headers Completed";
				m_error = DoReadHeadersComplete(m_error);
				break;
			case State::HTTPSTREAM_READ_BODY:
				m_error = DoReadBody(m_error);
				break;
			case State::HTTPSTREAM_READ_BODY_COMPLETED:
				m_error = DoReadBodyComplete(m_error);
				break;
			case State::HTTPSTREAM_DRAIN_BODY:
				m_error = DoDrainBody(m_error);
				break;
			case State::HTTPSTREAM_DRAIN_BODY_COMPLETED:
				m_error = DoDrainBodyComplete(m_error);
				break;
			case State::HTTPSTREAM_REQUEST_COMPLETED:
				DLOG(LOG_DEBUG) << "HTTPSTREAM: Request Completed";
				m_error = DoRequestComplete(m_error);
				break;
			default:
				break;
			}

		} while ((m_error != Psio::Network::Err::IO_PENDING) && (m_state != State::HTTPSTREAM_NONE && m_state != State::HTTPSTREAM_REQUEST_COMPLETED));

		return m_error;

	}

	int HttpStream::DoSendHeaders(int result)
	{
		m_next_state = State::HTTPSTREAM_SEND_HEADERS_COMPLETED;
		return m_sock->Write(m_req_buffer, [&](int res) -> int {
			CallDoLoop(res);
			return -1;
			});
	}

	int HttpStream::DoSendHeadersComplete(int result)
	{
		if (result < 0) {
			return result;
		}
		m_sent_bytes += result;
		if (m_req_buffer->Remaining() > 0) {
			// not all data was written to stream
			m_next_state = State::HTTPSTREAM_SEND_HEADERS;
			return Psio::Network::Err::OK; // goes back to loop calling write again.
		}
		// for now we assume no body to send
		m_next_state = State::HTTPSTREAM_SEND_HEADERS_AND_BODY_COMPLETE;
		return Psio::Network::Err::OK;
	}

	int HttpStream::DoSendBody(int result)
	{
		return result;
	}

	int HttpStream::DoSendBodyComplete(int result)
	{
		return result;
	}

	int HttpStream::DoSendAllComplete(int result)
	{
		if (m_req_buffer) {
			delete m_req_buffer;
			m_req_buffer = nullptr;
		}
		return result;
	}

	int HttpStream::DoReadHeaders(int result)
	{
		if (result < 0)
			return result;

		m_next_state = State::HTTPSTREAM_READ_HEADERS_COMPLETED;
		if (!m_res_buffer)
			m_res_buffer = new IOBuffer(2048);

		return m_sock->Read(m_res_buffer, [&](int result) -> int {
			CallDoLoop(result);
			return -1;
			});
	}

	// from chromium net/http/http_util.cc
	static size_t LocateEndOfHeadersHelper(const char* buf, size_t buf_len, size_t i, bool accept_empty_header_list) {
		char last_c = '\0';
		bool was_lf = false;
		if (accept_empty_header_list) {
			// Normally two line breaks signal the end of a header list. An empty header
			// list ends with a single line break at the start of the buffer.
			last_c = '\n';
			was_lf = true;
		}

		for (; i < buf_len; ++i) {
			char c = buf[i];
			if (c == '\n') {
				if (was_lf)
					return i + 1;
				was_lf = true;
			}
			else if (c != '\r' || last_c != '\n') {
				was_lf = false;
			}
			last_c = c;
		}
		return std::string::npos;
	}

	int HttpStream::DoReadHeadersComplete(int result)
	{
		size_t end_offset = std::string::npos;

		if (result < 0) {
			m_next_state = State::HTTPSTREAM_REQUEST_COMPLETED;
			return result;
		}

		if (result == 0) {
			// EOF - end the request
			m_next_state = State::HTTPSTREAM_REQUEST_COMPLETED;

			// TODO check for ssl - no truncated headers allowed
			bool ssl = false;

			// if no http status line was found treat it as a http0.9 message
			if (!ssl && m_headers_start == std::string::npos) {
				m_next_state = State::HTTPSTREAM_NONE;
				m_res_headers = HttpHeaders::CreateFromBuf(std::string("HTTP/0.9 200 OK"));
				return Network::Err::OK;
			}
			else {
				// truncated headers are not allowed
				return Network::Err::RESPONSE_HEADERS_TRUNCATED;
			}

			result = Network::Err::OK;
		}

		// search for start of status line
		if (m_headers_start == std::string::npos) {
			char* buf = m_res_buffer->data();
			size_t buf_len = m_res_buffer->Remaining();
			const size_t slop = 4;
			const size_t http_len = 4;

			if (buf_len >= http_len) {
				size_t i_max = min(buf_len - http_len, slop);
				for (size_t i = 0; i <= i_max; ++i) {
					if (!strncmp_i(buf + i, "http", http_len)) {
						m_headers_start = i;
						break;
					}
				}
			}

			// start of status still not found? http/0.9?
			if (m_headers_start == std::string::npos) {
				if (m_res_buffer->Remaining() >= 8) {
					// not found - if 8 bytes are read assume its a http/0.9 request
					// 4 bytes of junk and 4 bytes for "http"
					m_res_headers = HttpHeaders::CreateFromBuf(std::string("HTTP/0.9 200 OK"));
					return Network::Err::OK;
				}
				else {
					// no, simply too few data
					m_next_state = State::HTTPSTREAM_READ_HEADERS;
					return Network::Err::OK;
				}
			}
		}

		// start is found
		
		// search for end of headers
		end_offset = LocateEndOfHeadersHelper(m_res_buffer->data(), m_res_buffer->Remaining(), 0, false);

		// end of headers not found - too few data
		if (end_offset == std::string::npos) {
			m_next_state = State::HTTPSTREAM_READ_HEADERS;
			// safety check for too large headers
			if (end_offset > 256 * 1024)
				return Network::Err::RESPONSE_HEADERS_TOO_BIG;
			return Network::Err::IO_PENDING;
		}

		// parse headers
		m_res_buffer->Consume(m_headers_start);
		size_t header_len = end_offset - m_headers_start;
		m_res_headers = HttpHeaders::CreateFromBuf(std::string(m_res_buffer->reader(), header_len));
		if (!m_res_headers)
			return Network::Err::INVALID_HTTP_RESPONSE;
		m_res_buffer->Consume(header_len);

		// statistics
		m_received_bytes += end_offset;

		// calculate response body length and complete if no response body
		DetectBodyLength();
		if (m_response_length == 0)
			m_next_state = State::HTTPSTREAM_REQUEST_COMPLETED;

		return Network::Err::OK;
	}

	int HttpStream::DoReadBody(int result)
	{
		m_next_state = State::HTTPSTREAM_READ_BODY_COMPLETED;

		// data already sent with headers?
		if (m_res_buffer->Remaining()) {
			m_response_bytes_read += m_res_buffer->Remaining();
			m_user_buffer->Write(m_res_buffer->reader(), m_res_buffer->Remaining());
			m_res_buffer->Consume(m_res_buffer->Remaining());
		}

		if (IsBodyComplete()) {
			return Network::Err::OK;
		}

		return m_sock->Read(m_user_buffer, [&](int res) -> int {
			CallDoLoop(res);
			return -1;
			});
	}

	int HttpStream::DoReadBodyComplete(int result)
	{
		if (result < 0)
			return result;

		m_response_bytes_read += result;

		if (!result || IsBodyComplete())
			m_next_state = State::HTTPSTREAM_REQUEST_COMPLETED;
		else
			m_next_state = State::HTTPSTREAM_READ_BODY;

		return Network::Err::OK;
	}

	int HttpStream::DoDrainBody(int result)
	{
		m_next_state = State::HTTPSTREAM_READ_BODY_COMPLETED;

		// data already sent with headers?
		if (m_res_buffer->Remaining()) {
			m_response_bytes_read += m_res_buffer->Remaining();
			m_res_buffer->Consume(m_res_buffer->Remaining());
		}

		if (IsBodyComplete()) {
			return Network::Err::OK;
		}

		return m_sock->Read(m_user_buffer, [&](int res) -> int {
			CallDoLoop(res);
			return -1;
			});
	}

	int HttpStream::DoDrainBodyComplete(int result)
	{
		m_response_bytes_read += result;

		if (!result || IsBodyComplete())
			m_next_state = State::HTTPSTREAM_REQUEST_COMPLETED;
		else
			m_next_state = State::HTTPSTREAM_DRAIN_BODY;

		return Network::Err::OK;
	}

	bool HttpStream::IsBodyComplete()
	{
		// TODO chunked?

		//  if we have a length compare to bytes read
		if (m_response_bytes_read >= m_response_length)
			return true;

		// else we need to wait on EOF
		return false;
	}

	// unused yet
	int HttpStream::DoRequestComplete(int result)
	{
		m_next_state = State::HTTPSTREAM_REQUEST_COMPLETED;
		return result;
	}
	
	int HttpStream::ParseResponseHeaders(size_t end_offset)
	{
		m_received_bytes += end_offset;
		return 0;
	}

	void HttpStream::DetectBodyLength()
	{
		// Try to detect the response length

		// For HEAD we dont expect response body in any case
		if (!m_request.headers.GetMethod().compare("HEAD"))
			m_response_length = 0;

		// For certain response code the body MUST be 0
		else if (m_res_headers->GetResponseCode() / 100 == 1)
			m_response_length = 0;
		else 
			switch (m_res_headers->GetResponseCode()) {
			case 204:  // No Content
			case 205:  // Reset Content
			case 304:  // Not Modified
				m_response_length = 0;
				break;
			}

		if (m_response_length == -1) {
			// TODO: chunked transfer
			std::string len = (*m_res_headers)["Content-Length"];
			if (!len.empty()) {
				m_response_length = std::stoi(len);
			}
		}
	}

	
}