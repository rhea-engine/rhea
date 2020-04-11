#include <stdio.h>

#include <string>

#include "Iris.h"
#include "Hermes/Core.h"
#include "Hermes/IPAddress.h"
#include "Hermes/Server.h"
#include "Hermes/Connection.h"
#include "RegistryMemory.h"
#include "Middleware.h"
#include "Components/Log/Log.h"

struct Config {
	uint16_t port;
	bool tcp;
	bool udp;
	std::string registry_file;
};

void Iris::init(Config config)
{
	m_registry = new RegistryMemory();
	m_core = new Hermes::Core();
	m_log = new Log();

	// add a default value for server
	Entry server_entry ("Server", "meworla/registry");
	server_entry.setValue("version", "0.1");

	m_registry->insert("/", &server_entry);

	if (!config.registry_file.empty()) {
		static_cast<RegistryMemory*>(m_registry)->setRegistryFile(config.registry_file);
		int entries = static_cast<RegistryMemory*>(m_registry)->load();
		m_log->info("Import registry from %s: %d entries", config.registry_file.c_str(), entries);
	}

	Hermes::IPAddress ip;
	m_server = new Hermes::Server(config.tcp, config.udp);
	m_middleware = new Middleware(this, m_registry);

	m_server->setRequestHandlerFactory(m_middleware);
	m_server->addConnectionHandler(this);
	if (m_server->listen(ip, config.port) != 0) {
		m_log->info("failed to start server\n");
		return;
	}
	m_core->addTask(m_server);
	m_log->info("Started Server listening on port %d\n", config.port);
}

int Iris::loop()
{
	return m_core->loop();
}

void Iris::close()
{
	delete m_core;
}

void Iris::onConnection(Hermes::Connection* conn)
{
	m_log->set(Log::LOG_HOST, conn->get_remote_s());
	m_log->info("Connected on port %d", conn->get_port());
}

void Iris::onClose(Hermes::Connection* conn)
{
	m_log->info("Connection closed");
}

int main(int argc, char* argv[])
{
	Iris* app = new Iris();
	int argi = 1;
	Config config;

	//  default configuration
	config.port = 27000;
	config.tcp = true;
	config.udp = false;	

	while (argi < argc) {		
		if (!strcmp(argv[argi], "-p")) {
			argi++;
			if (argi < argc)
				config.port = atoi(argv[argi]);
		}
		else if (!strcmp(argv[argi], "-r")) {
			argi++;
			if (argi < argc)
				config.registry_file = std::string(argv[argi]);
		}
		else if (!strcmp(argv[argi], "-tcp"))
			config.tcp = false;
		else if (!strcmp(argv[argi], "+udp"))
			config.udp = true;
		argi++;
	}
	app->init(config);

	int ret = app->loop();

	app->close();

	return ret;
}