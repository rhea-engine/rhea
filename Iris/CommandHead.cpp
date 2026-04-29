/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "CommandHead.h"

int CommandHead::OnRequest(psio::HttpRequest* req, psio::HttpResponse* res)
{
	return res->end();
}
