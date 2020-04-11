#include "CommandHello.h"
#include "Hermes/Connection.h"

void CommandHello::onRequest(std::shared_ptr<Request> request, std::shared_ptr<Response> response)
{
	if (!response)
		return;

	unsigned char answer[] = "\rhello\r\n";
	response->write(answer, sizeof(answer));
	response->end();

	return;
}