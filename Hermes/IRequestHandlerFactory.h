#ifndef HERMES_REQUESTHANDLERFACTORY_H
#define HERMES_REQUESTHANDLERFACTORY_H

#include <memory>

#include "Request.h"
#include "Response.h"
#include "IRequestHandler.h"

namespace Hermes
{
	class IRequestHandlerFactory
	{
	public:
		// handle the request after receiving the header
		virtual IRequestHandler* onRequest(std::shared_ptr<Request> request) = 0;
	};
}

#endif // !HERMES_REQUESTHANDLERFACTORY_H
