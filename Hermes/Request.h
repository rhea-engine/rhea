#ifndef Hermes_REQUEST_H
#define Hermes_REQUEST_H

#include <string>
#include <memory>

class IRequestHandler;

namespace Hermes
{
	class Connection;
	class IRequestHandler;

	class Request : public std::enable_shared_from_this<Request>
	{
	public:
		enum REQ_METHOD {
			METHOD_UNKNOWN,
			METHOD_GET,
			METHOD_QUIT,
			METHOD_HELLO
		};

		Request(Connection* connection) :
			m_buf(NULL),
			m_connection(connection),
			m_req_handler(NULL)
		{
		}
		~Request();
		void set_buf(unsigned char* buf) { m_buf = buf; }
		unsigned char* get_buf() { return m_buf; }		
		// set the command 
		void setCommand(std::string method, std::string value);
		// return the command
		std::string getCommand() { return m_command; }
		// return the value / url
		std::string getValue() { return m_value; }
		// clear the request after finishing it
		void clear();	
		// set request Handler
		void setRequestHandler(IRequestHandler* req_handler) { m_req_handler = req_handler; }
	
		// handle onData event
		void onData(unsigned char* chunk, size_t chunk_len);
		
		// handle onEnd event
		void onEnd();
	protected:
		Connection* m_connection;
		unsigned char* m_buf;
		std::string m_command;
		std::string m_value;	
		IRequestHandler* m_req_handler;
	};
}

#endif // !Hermes_REQUEST_H
