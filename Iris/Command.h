/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef COMMAND_H
#define COMMAND_H

#include <Psio/Net/Http/HttpResponse.h>
#include <Psio/Net/Http/HttpRequest.h>

// Base class for commands with no body 
class Command
{
public:
	~Command()
	{
	}

	// handle request
	virtual int OnRequest(psio::HttpRequest* req, psio::HttpResponse* res) = 0;
	
	void onData(unsigned char* buf, size_t len) {}
	void onEnd() { }

	// handle end of request
	void onRequestEnd() { delete this; }
};

#endif // !COMMAND_H