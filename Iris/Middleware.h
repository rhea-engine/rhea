/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef IRISS_PARSER_H
#define IRISS_PARSER_H

#include <memory>
#include <unordered_map>
#include <string>

class Command;
class Registry;

namespace psio
{
	class HttpRequest;
	class HttpResponse;
}

class Middleware 
{
public:	
	Middleware(Registry *registry);
	
	// handle the request after receiving the header
	int onRequest(psio::HttpRequest* req, psio::HttpResponse* res);
protected:
	std::unordered_map<std::string, int> m_command_table;
	Registry* m_registry;
};

#endif // !IRISSESSION_H