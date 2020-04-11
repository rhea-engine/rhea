#include "CommandQuit.h"
#include "Hermes/Connection.h"

void CommandQuit::onRequest(std::shared_ptr<Request> request, std::shared_ptr<Response> response)
{
	if (!m_res)
		return;

	unsigned char answer[] = "\rExiting...\r\n";
	m_res->write(answer, sizeof(answer));
	m_res->end();

	return;
}
