#ifndef COMMAND_HEAD_H
#define COMMAND_HEAD_H

#include "Command.h"

class CommandHead : public Command
{
public:
	void onRequest(std::shared_ptr<Request> request, std::shared_ptr<Response> response);
};

#endif // !COMMAND_HEAD_H
