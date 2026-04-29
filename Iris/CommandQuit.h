/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef COMMAND_QUIT_H
#define COMMAND_QUIT_H

#include "Command.h"

class CommandQuit : public Command
{
public:
	int OnRequest(psio::HttpRequest* req, psio::HttpResponse* res);
};

#endif // !COMMAND_QUIT_H
