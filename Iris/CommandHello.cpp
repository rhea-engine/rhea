/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "CommandHello.h"

int CommandHello::OnRequest(psio::HttpRequest* req, psio::HttpResponse* res)
{
	char answer[] = "\rhello\r\n";
	res->Write(answer, strlen(answer));
	return res->end();
}
