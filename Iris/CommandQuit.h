#ifndef COMMAND_QUIT_H
#define COMMAND_QUIT_H

#include "Command.h"

class CommandQuit : public Command
{
public:
	void onRequest(std::shared_ptr<Request> request, std::shared_ptr<Response> response);
};

#endif // !COMMAND_QUIT_H
