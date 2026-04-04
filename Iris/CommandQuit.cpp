#include "CommandQuit.h"

int CommandQuit::OnRequest(psio::HttpRequest* req, psio::HttpResponse* res)
{
	char answer[] = "\rExiting...\r\n";
	res->Write(answer, sizeof(answer));
	return res->end();
}
