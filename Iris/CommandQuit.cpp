#include "CommandQuit.h"

int CommandQuit::OnRequest(Psio::HttpRequest* req, Psio::HttpResponse* res)
{
	char answer[] = "\rExiting...\r\n";
	res->Write(answer, sizeof(answer));
	return res->end();
}
