#ifndef COMMAND_H
#define COMMAND_H

#include "Hermes/IRequestHandler.h"

using Hermes::Response;
using Hermes::Request;

// Base class for commands with no body 
class Command : public Hermes::IRequestHandler
{
public:

	Command() :
		m_req(0),
		m_res(0)
	{}
	~Command()
	{
	}

	// handle request
	void onRequest(std::shared_ptr<Request> request, std::shared_ptr<Response> response) { m_res = response; m_req = request; }
	
	void onData(unsigned char* buf, size_t len) {}
	void onEnd() { }

	// handle end of request
	void onRequestEnd() { delete this; }
	
	// get the current Response object
	std::shared_ptr<Response> getResponse() { return m_res; }	

protected:
	std::shared_ptr<Hermes::Request> m_req;
	std::shared_ptr<Hermes::Response> m_res;
};

#endif // !COMMAND_H