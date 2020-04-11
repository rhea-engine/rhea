#include "Request.h"
#include "Response.h"
#include "IRequestHandler.h"
#include "Connection.h"

namespace Hermes
{
	void Request::setCommand(std::string command, std::string value)
	{
		m_command = command;
		m_value = value;
	}

	void Request::onData(unsigned char* chunk, size_t chunk_len)
	{
		if (m_req_handler)
			m_req_handler->onData(chunk, chunk_len);
	}

	void Request::onEnd()
	{
		if (m_req_handler)
			m_req_handler->onEnd(); 
	}

	void Request::clear()
	{
		if (m_req_handler) {
			m_req_handler->onRequestEnd();
			m_req_handler = NULL;
		}
	}

	Request::~Request()
	{
		clear();
	}
}