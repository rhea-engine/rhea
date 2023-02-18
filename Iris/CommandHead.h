#ifndef COMMAND_HEAD_H
#define COMMAND_HEAD_H

#include "Command.h"

class CommandHead : public Command
{
public:
	int OnRequest(Psio::HttpRequest* req, Psio::HttpResponse* res);
};

#endif // !COMMAND_HEAD_H
