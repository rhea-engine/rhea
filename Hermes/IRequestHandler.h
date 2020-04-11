#ifndef HERMES_REQUESTHANDLER_H
#define HERMES_REQUESTHANDLER_H

#include <memory>

#include "Request.h"
#include "Response.h"

namespace Hermes
{
	class IRequestHandler
	{
	public:
		// handle the request after receiving the header
		virtual void onRequest(std::shared_ptr<Request> request, std::shared_ptr<Response> response) = 0;

		// handle data stream
		virtual void onData(unsigned char*, size_t) = 0;

		// handle end of input
		virtual void onEnd() = 0;

		// handle RequestEnd (request will be destroyed after)
		virtual void onRequestEnd() = 0;
	};
}

#endif // !HERMES_REQUESTHANDLER_H
