#ifndef COMMAND_HEAD_H
#define COMMAND_HEAD_H

#include "Command.h"

class CommandHead : public Command
{
public:
	int OnRequest(psio::HttpRequest* req, psio::HttpResponse* res);
};

#endif // !COMMAND_HEAD_H
