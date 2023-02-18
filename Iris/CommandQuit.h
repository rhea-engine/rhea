#ifndef COMMAND_QUIT_H
#define COMMAND_QUIT_H

#include "Command.h"

class CommandQuit : public Command
{
public:
	int OnRequest(Psio::HttpRequest* req, Psio::HttpResponse* res);
};

#endif // !COMMAND_QUIT_H
