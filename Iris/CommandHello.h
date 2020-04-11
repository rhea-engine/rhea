#ifndef HELLO_COMMAND_H
#define HELLO_COMMAND_H

#include "Command.h"

class CommandHello : public Command
{
public:
	void onRequest(std::shared_ptr<Request> request, std::shared_ptr<Response> response);
};

#endif // !HELLO_COMMAND_H
