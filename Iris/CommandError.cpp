#include "CommandError.h"

int CommandError::OnRequest(psio::HttpRequest* req, psio::HttpResponse* res)
{
	return res->sendError(501);
}
