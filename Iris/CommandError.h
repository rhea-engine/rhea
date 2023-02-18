#ifndef COMMAND_ERROR_H
#define COMMAND_ERROR_H

#include "Command.h"

class Registry;
class Entry;

class CommandError : public Command
{
public:
	CommandError(int error_code) :
		m_error_code(error_code)
	{}
	// IRequestHandler functions
	int OnRequest(Psio::HttpRequest* req, Psio::HttpResponse* res);
protected:
	int m_error_code;
};

#endif // !COMMAND_ERROR_H
