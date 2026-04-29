/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

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
	int OnRequest(psio::HttpRequest* req, psio::HttpResponse* res);
protected:
	int m_error_code;
};

#endif // !COMMAND_ERROR_H
