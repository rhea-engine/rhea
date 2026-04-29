/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef HELLO_COMMAND_H
#define HELLO_COMMAND_H

#include "Command.h"

class CommandHello : public Command
{
public:
	int OnRequest(psio::HttpRequest* req, psio::HttpResponse* res);
};

#endif // !HELLO_COMMAND_H
