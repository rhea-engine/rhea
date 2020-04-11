#ifndef Hermes_HTTP_PARSER
#define Hermes_HTTP_PARSER

#include <stdio.h>
#include <memory>
#include "IProtocol.h"

namespace Hermes
{
	class Request;
	class HttpResponse;

	class Http : public IProtocol
	{		
	public:
		enum HTTP_STATE {
			HTTP_NONE,
			HTTP_REQUEST,
			HTTP_HEADER,
			HTTP_BODY,
			HTTP_FINISHED
		};
		Http() :			
			m_state(HTTP_NONE),
			m_echo(0),
			m_req(NULL),
			m_res(NULL),
			m_connection(NULL),
			msg(NULL),
			echo_buf(NULL),
			m_reader(0),
			m_len(0),
			m_buflen(0)
		{}
		~Http();

		// set the request to add infos to
		void setRequest(std::shared_ptr<Request>& req) {
			m_req = req; m_state = HTTP_NONE;
		}
		
		// IProtocol

		// call Protocol on connection with the connection object
		virtual void onConnection(Connection* conn);
		// data is available
		virtual void onData(unsigned char* buf, size_t buflen);
	protected:
		// cleanup buffer and search for newline
		unsigned char* getLine(unsigned char* buf, size_t& reader, int& buflen);
		// parse one header line
		bool parseRequestLine(unsigned char* buf, unsigned char *end_p);
		// parse the request line
		bool parseHeaderLine(unsigned char* buf, unsigned char* end_p);
		// echo buf back to connection stripping NVT commands
		void echo(unsigned char* buf, size_t len);

		std::shared_ptr<Request> m_req;
		std::shared_ptr<HttpResponse> m_res;
		HTTP_STATE m_state;
		Connection* m_connection;
		unsigned char* msg;
		size_t echo_len = 0;
		unsigned char* echo_buf;
		bool m_echo;
		unsigned char* reader;
		size_t m_reader;
		int m_len;
		size_t m_buflen;
		bool m_has_body = false;
		size_t m_body_len = 0;
	};
}
#endif // !Hermes_HTTP_PARSER
