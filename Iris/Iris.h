#ifndef IRIS_H
#define IRIS_H

#include <stdint.h>
#include <memory>

#include <Psio/Net/Network.h>
#include <Psio/Net/Socket/TCPSocket.h>

class Registry;
class Middleware;
class Log;

struct Config;;

class Iris
{
public:
	Iris() :
		m_middleware(NULL),
		m_registry(NULL)
	{}
	void init(Config config);
	int loop();

protected:
	Registry* m_registry;
	std::unique_ptr<Psio::TCPSocket> m_server_sock;
	Middleware* m_middleware;
	std::unique_ptr<Psio::Network> m_network;

};

#endif // !IRIS_H
