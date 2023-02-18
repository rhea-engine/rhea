#include "CommandError.h"

int CommandError::OnRequest(Psio::HttpRequest* req, Psio::HttpResponse* res)
{
	return res->sendError(501);
}
