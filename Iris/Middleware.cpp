#include <string>

#include "Middleware.h"
#include "CommandHello.h"
#include "CommandQuit.h"
#include "CommandGet.h"
#include "CommandError.h"
#include <Psio/Net/Http/HttpRequest.h>
#include <Psio/Net/Http/HttpResponse.h>

enum CMD_ID {
	CMD_HELLO,
	CMD_QUIT,
	CMD_GET
};

Middleware::Middleware(Registry* registry) : 
	m_registry(registry)
{
	m_command_table["hello"] = CMD_HELLO;
	m_command_table["quit"] = CMD_QUIT;
	m_command_table["get"] = CMD_GET;
}

int Middleware::onRequest(psio::HttpRequest* req, psio::HttpResponse* res)
{
	Command* r_cmd = nullptr;
	std::string str = req->headers.GetMethod();
	// lowercase the cmd (must be ascii)
	for (int i = 0; str[i] != 0; i++)
		if (str[i] <= 'Z' && str[i] >= 'A')
			str[i] += 32;

	switch(m_command_table[str]) {
	case CMD_HELLO:
		r_cmd = new CommandHello();
		break;
	case CMD_QUIT:
		r_cmd = new CommandQuit();
		break;
	case CMD_GET:
		r_cmd = new CommandGet(m_registry);
		break;
	default:
		// unknown command - send not implemented error	
		r_cmd = new CommandError(501);
	}

	return r_cmd->OnRequest(req, res);
}
