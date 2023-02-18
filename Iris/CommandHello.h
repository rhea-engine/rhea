#ifndef HELLO_COMMAND_H
#define HELLO_COMMAND_H

#include "Command.h"

class CommandHello : public Command
{
public:
	int OnRequest(Psio::HttpRequest* req, Psio::HttpResponse* res);
};

#endif // !HELLO_COMMAND_H
