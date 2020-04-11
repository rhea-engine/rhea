#ifndef IRISS_PARSER_H
#define IRISS_PARSER_H

#include <memory>
#include <unordered_map>
#include <string>

#include "Hermes/IRequestHandlerFactory.h"


class Command;
class Iris;
class Registry;

namespace Hermes
{
	class Request;
	class Response;
}

class Middleware : public Hermes::IRequestHandlerFactory
{
public:	
	Middleware(Iris *iris, Registry *registry);
	
	// handle the request after receiving the header
	Hermes::IRequestHandler* onRequest(std::shared_ptr<Hermes::Request> request);
protected:
	std::unordered_map<std::string, int> m_command_table;
	Iris* m_iris;
	Registry* m_registry;
};

#endif // !IRISSESSION_H