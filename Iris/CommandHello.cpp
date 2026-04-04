#include "CommandHello.h"

int CommandHello::OnRequest(psio::HttpRequest* req, psio::HttpResponse* res)
{
	char answer[] = "\rhello\r\n";
	res->Write(answer, strlen(answer));
	return res->end();
}
