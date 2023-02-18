#include "CommandHead.h"

int CommandHead::OnRequest(Psio::HttpRequest* req, Psio::HttpResponse* res)
{
	return res->end();
}
