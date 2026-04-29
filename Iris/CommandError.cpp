/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "CommandError.h"

int CommandError::OnRequest(psio::HttpRequest* req, psio::HttpResponse* res)
{
	return res->sendError(501);
}
