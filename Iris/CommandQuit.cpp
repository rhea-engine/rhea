/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "CommandQuit.h"

int CommandQuit::OnRequest(psio::HttpRequest* req, psio::HttpResponse* res)
{
	char answer[] = "\rExiting...\r\n";
	res->Write(answer, sizeof(answer));
	return res->end();
}
