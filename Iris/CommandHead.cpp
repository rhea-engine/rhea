#include "CommandHead.h"

int CommandHead::OnRequest(psio::HttpRequest* req, psio::HttpResponse* res)
{
	return res->end();
}
