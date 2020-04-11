#ifndef IRIS_H
#define IRIS_H

#include <stdint.h>
#include <memory>

#include "Hermes/IConnectionHandler.h"

namespace Hermes
{
	class Core;
	class Server;
	class Connection;
}
class Registry;
class Middleware;
class Log;

struct Config;;

class Iris : public Hermes::IConnectionHandler
{
public:
	Iris() :
		m_server(NULL),
		m_middleware(NULL),
		m_registry(NULL)
	{}
	void init(Config config);
	int loop();
	void close();

	// get the log facility
	Log* getLog() { return m_log; }

	// connection handler
	void onConnection(Hermes::Connection* conn);
	void onClose(Hermes::Connection* conn);
protected:
	Hermes::Core *m_core;
	Registry* m_registry;
	Hermes::Server* m_server;
	Middleware* m_middleware;
	Log* m_log;
};

#endif // !IRIS_H
