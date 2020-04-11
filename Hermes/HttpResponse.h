#ifndef MWNET_HTTP_RESPONSE
#define MWNET_HTTP_RESPONSE

#include <memory>
#include "Response.h"

namespace Hermes
{
	class HttpResponse : public Response
	{
	public:
		HttpResponse(Connection* conn, std::shared_ptr<Request> request);

		// write all headers
		void writeHeaders();
		// send error with message
		void sendErrorMessage(int error_code, unsigned char* error_message);
	};
}
#endif // !MWNET_HTTP_RESPONSE
