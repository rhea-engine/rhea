/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef COMMAND_GET_H
#define COMMAND_GET_H

#include "Command.h"

class Registry;
class Entry;
class Log;

class CommandGet : public Command
{
public:
	CommandGet(Registry* registy) :
		m_registry(registy)
	{}

	int OnRequest(psio::HttpRequest* req, psio::HttpResponse* res);

protected:
	Registry* m_registry;
	std::string m_path;
};

#endif // !COMMAND_GET_H
