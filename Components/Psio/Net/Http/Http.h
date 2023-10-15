#ifndef PSIO_HTTP_PARSER
#define PSIO_HTTP_PARSER

#include <stdio.h>
#include <memory>
#include <Psio/Net/Socket/StreamSocket.h>
#include "HttpHeaders.h"
#include "HttpResponse.h"
#include "HttpRequest.h"
#include <Psio/Net/IOBuffer.h>

namespace Psio
{
	//class Request;
	class HttpResponse;
	class HttpRequest;

	class Http
	{		
	public:
		// input states
		enum HTTP_STATE {
			HTTP_NONE,
			HTTP_TERMINALCHECK,
			HTTP_TERMINALCHECK_COMPLETED,
			HTTP_REQUEST,
			HTTP_REQUEST_COMPLETED,
			HTTP_HEADER,
			HTTP_HEADER_COMPLETED,
			HTTP_BODY,
			HTTP_BODY_COMPLETED,			
			HTTP_FINISHED
		};

		enum HTTP_SEND_STATE {
			HTTP_SEND_NONE,
			HTTP_SEND_PENDING, // sending is pending, until request is reveived fully
			HTTP_SEND,
			HTTP_SEND_COMPLETED,
			HTTP_SEND_FINISHED
		};
		Http() :			
			m_state(HTTP_NONE),
			m_next_state(HTTP_NONE),
			m_send_state(HTTP_SEND_PENDING),			
			m_next_send_state(HTTP_SEND_FINISHED),
			m_echo(0),
			msg(NULL),
			m_res_end(false),
			echo_buf(NULL),
			m_headers(nullptr),
			m_ondata_handler({}),
			m_onrequest_handler({})
		{}
		~Http();

		typedef std::function<int(HttpRequest*, HttpResponse*)> RequestHandler;
		void OnData(Network::CompletionHandler data_handler);
		void OnClose(Network::CompletionHandler onclose_handler) { m_onclose_handler = onclose_handler; }
		
		// call Protocol on request with the response object
		int OnRequest(std::unique_ptr<Psio::StreamSocket> new_sock, RequestHandler onrequest_handler);

		// send chunk of data
		int Write(const char* buf, size_t buflen, bool no_buffering = true);

		// end response
		// returns IO_PENDING or bytes sent with last write
		// if it is not io_pending the request has been sent synchronously
		int ResponseEnd();

	protected:
		// parse the request line
		int parseRequestLine(unsigned char* buf, unsigned char *end_p, HttpHeaders* headers);

		// echo buf back to connection stripping NVT commands
		void echo(unsigned char* buf, size_t len);

		int DoSendLoop(int result);
		void CallSendLoop(int result);
		int DoWrite(int result);
		int DoWriteCompleted(int result);
		int DoEnd(int result);

		HTTP_SEND_STATE m_send_state;
		HTTP_SEND_STATE m_next_send_state;

		RequestHandler m_onrequest_handler;
		Network::CompletionHandler m_ondata_handler;
		Network::CompletionHandler m_onclose_handler;

		int DoLoop(int result);
		void CallDoLoop(int result);
		int DoTerminalCheck(int result);
		int DoTerminalCheckCompleted(int result);
		int DoRequestLine(int result);
		int DoRequestLineCompleted(int result);
		int DoHeaders(int result);
		int DoHeadersCompleted(int result);
		int DoBody(int result);
		int DoBodyCompleted(int result);		
		int DoResponse(int result);

		HTTP_STATE m_state;
		HTTP_STATE m_next_state;

		unsigned char* msg;
		int echo_len = 0;
		unsigned char* echo_buf;
		bool m_echo;
		bool m_has_body = false;
		size_t m_body_len = 0;
		size_t m_body_read = 0;

		bool m_res_end;

		HttpHeaders* m_headers;

		std::unique_ptr<Psio::StreamSocket> m_sock;
		std::unique_ptr<Psio::IOBuffer> m_inbuf;
		std::unique_ptr<Psio::IOBuffer> m_outbuf;

		std::unique_ptr<HttpRequest> m_request;
		std::unique_ptr<HttpResponse> m_response;
	};
}
#endif // !PSIO_HTTP_PARSER
