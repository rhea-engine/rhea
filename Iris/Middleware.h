#ifndef IRISS_PARSER_H
#define IRISS_PARSER_H

#include <memory>
#include <unordered_map>
#include <string>

class Command;
class Registry;

namespace Psio
{
	class HttpRequest;
	class HttpResponse;
}

class Middleware 
{
public:	
	Middleware(Registry *registry);
	
	// handle the request after receiving the header
	int onRequest(Psio::HttpRequest* req, Psio::HttpResponse* res);
protected:
	std::unordered_map<std::string, int> m_command_table;
	Registry* m_registry;
};

#endif // !IRISSESSION_H