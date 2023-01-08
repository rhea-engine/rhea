#ifndef PSIO_NET_HTTP_HTTPSTREAM_H
#define PSIO_NET_HTTP_HTTPSTREAM_H

#include <string>
#include "HttpRequest.h"
#include "../Network.h"
#include "../IOBuffer.h"
#include "../Socket/Socket.h"

namespace Psio
{
	class HttpStream {
	public:
		// constructor takes a connected(!) socket
		HttpStream(std::unique_ptr<Socket> sock) :
			m_sock(std::move(sock)),
			m_state(State::HTTPSTREAM_NONE),
			m_next_state(State::HTTPSTREAM_NONE),
			m_sent_bytes(0),
			m_received_bytes(0),
			m_response_bytes_read(0),
			m_req_buffer(nullptr),
			m_res_buffer(nullptr),
			m_user_buffer(nullptr),
			m_user_buffer_len(0),
			m_res_headers(nullptr),
			m_headers_start(std::string::npos),
			m_handler(NULL),
			m_response_length(-1)
		{}
		~HttpStream();

		/* Sets up a connection to the endpoint and sends the request
		* 
		* If IO_PENDING is returned the handler is called as soon as the request was sent and the response
		* is ready for reading.
		* Any other negative value is an error
		*/
		int SendRequest(HttpRequest& request, Psio::Network::CompletionHandler handler);

		/* Reads a set of Http headers
		* 
		* This is normaly only called once per request. The case for 1xx headers is noct yet implemented
		* 
		* If IO_Pending is returned the handler is calles as soon as the header set is read
		* Any other negative value is an error
		*/
		int ReadResponseHeaders(Network::CompletionHandler handler);

		/* Returns the headers
		* 
		*  Returns the headers of the response. NULL indicates no headers read or error while parsing them.
		*  Even HTTP/0.9 responses should have a header structure after reading the request.
		*/
		HttpHeaders* GetHeaders() { return m_res_headers; }

		/* Reads up to chunk_len bytes into buffer buf. 
		* 
		* If IO_PENDING is returned the handler is called as soon as up to chunk_len bytes where read. The number of actual bytes read may be lower.
		* Any other negative value is an error
		*/
		int ReadBody(IOBuffer* buf, size_t chunk_len, Psio::Network::CompletionHandler handler);

		/* Drain the response body
		* 
		*  Discards the response body. This is used if we do not read the body but want to reuse the socket
		* 
		* If IO_PENDING is returned the handler is called as soon as the body was drained. 
		* Any other negative value is an error
		*/
		int DrainBody(Psio::Network::CompletionHandler handler);

		void CallDoLoop(int result);

	protected:
		enum class State {
			HTTPSTREAM_NONE,
			HTTPSTREAM_SEND_HEADERS,
			HTTPSTREAM_SEND_HEADERS_COMPLETED,
			HTTPSTREAM_SEND_BODY,
			HTTPSTREAM_SEND_BODY_COMPLETED,
			HTTPSTREAM_SEND_HEADERS_AND_BODY_COMPLETE,
			HTTPSTREAM_READ_HEADERS,
			HTTPSTREAM_READ_HEADERS_COMPLETED,
			HTTPSTREAM_READ_BODY,
			HTTPSTREAM_READ_BODY_COMPLETED,
			HTTPSTREAM_DRAIN_BODY,
			HTTPSTREAM_DRAIN_BODY_COMPLETED,
			HTTPSTREAM_REQUEST_COMPLETED
		};

		// state var
		HttpStream::State m_state;
		HttpStream::State m_next_state;

		// state machine
		int DoLoop(int result);

		// state functions
		int DoSendHeaders(int result);
		int DoSendHeadersComplete(int result);
		int DoSendBody(int result);
		int DoSendBodyComplete(int result);
		int DoSendAllComplete(int result);
		int DoReadHeaders(int result);
		int DoReadHeadersComplete(int result);
		int DoReadBody(int result);
		int DoReadBodyComplete(int result);
		int DoDrainBody(int result);
		int DoDrainBodyComplete(int result);
		int DoRequestComplete(int result);

		// callbacks
		Network::CompletionHandler m_handler;

		// the socket reference
		std::unique_ptr<Socket> m_sock;

		// the request buffer
		IOBuffer* m_req_buffer;

		// the response buffer
		IOBuffer* m_res_buffer;

		// user buffer
		IOBuffer* m_user_buffer;
		size_t m_user_buffer_len;

		// stats
		size_t m_sent_bytes;
		size_t m_received_bytes;
		size_t m_response_bytes_read;

		// position of status line
		size_t m_headers_start;

		HttpRequest m_request;
		HttpHeaders* m_res_headers;
		int ParseResponseHeaders(size_t end_offset);

		// response length
		void DetectBodyLength();
		int m_response_length;
		bool IsBodyComplete();
	};
}

#endif // !PSIO_NET_HTTP_HTTPSTREAM_H
