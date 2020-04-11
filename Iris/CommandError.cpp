#include "CommandError.h"

void CommandError::onRequest(std::shared_ptr<Request> request, std::shared_ptr<Response> response)
{
	response->sendError(501);
}