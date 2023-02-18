#include "CommandHello.h"

int CommandHello::OnRequest(Psio::HttpRequest* req, Psio::HttpResponse* res)
{
	char answer[] = "\rhello\r\n";
	res->Write(answer, strlen(answer));
	return res->end();
}
