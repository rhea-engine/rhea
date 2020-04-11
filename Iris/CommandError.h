#ifndef COMMAND_ERROR_H
#define COMMAND_ERROR_H

#include "Command.h"
#include "IRegistryHandler.h"

class Registry;
class Entry;

class CommandError : public Command
{
public:
	CommandError(int error_code) :
		m_error_code(error_code)
	{}
	// IRequestHandler functions
	void onRequest(std::shared_ptr<Request> request, std::shared_ptr<Response> response);
protected:
	int m_error_code;
};

#endif // !COMMAND_ERROR_H
