/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <stdio.h>
#include <string>

#include "Iris.h"
#include <Psio/Net/Socket/IPAddress.h>
#include "RegistryMemory.h"
#include "Middleware.h"
#include <Psio/Log.h>
#include <Psio/Net/Http/Http.h>

struct Config {
	std::string ip;
	uint16_t port;
	bool tcp;
	bool udp;
	std::string registry_file;
};

void Iris::init(Config config)
{
	m_registry = new RegistryMemory();

	// add a default value for server
	Entry server_entry ("Server", "meworla/registry");
	server_entry.setValue("version", "0.1");

	m_registry->insert("/", &server_entry);

	if (!config.registry_file.empty()) {
		static_cast<RegistryMemory*>(m_registry)->setRegistryFile(config.registry_file);
		int entries = static_cast<RegistryMemory*>(m_registry)->load();
		LOG(LOG_INFO) << "Import registry from " << config.registry_file << ": " << entries << " entries";
	}

	m_network = std::make_unique<psio::Network>(nullptr);
	m_network->Init();
	m_server_sock = std::make_unique<psio::TCPSocket>(m_network.get());
	m_middleware = new Middleware(m_registry);

	int err = m_server_sock->Server(config.ip, config.port);
	LOG(LOG_INFO) << "Started Server. Listening on " << config.ip << ":" << config.port;
	m_server_sock->Accept([&](psio::StreamSocket* new_sock, int err) {
		if (err < 0) {
			LOG(LOG_ERROR) << "OnConnection failed: " << err;
			return;
		}
		std::unique_ptr<psio::StreamSocket> sock;
		sock.reset(new_sock);
		
		psio::Http* http = new psio::Http();
		int ret = http->OnRequest(std::move(sock), [&](psio::HttpRequest* req, psio::HttpResponse* res) -> int {
			return m_middleware->onRequest(req, res);
			});
		if (ret < -1) {
			delete http;
		}
		});
	if (err < 0) {
		LOG(LOG_FATAL) << "failed to start server";
		return;
	}
}

int Iris::loop()
{
	do
	{
		// iterate over connections
		m_network->RunLoop();

		// give time to the system
		Sleep(1);

	} while (true);
}

int main(int argc, char* argv[])
{
	Iris* app = new Iris();
	int argi = 1;
	Config config;

	//  default configuration
	config.ip = "127.0.0.1";
	config.port = 27000;
	config.tcp = true;
	config.udp = false;	

	while (argi < argc) {		
		if (!strcmp(argv[argi], "-p")) {
			argi++;
			if (argi < argc)
				config.port = atoi(argv[argi]);
		}
		else if (!strcmp(argv[argi], "-h")) {
			argi++;
			if (argi < argc)
				config.ip = std::string(argv[argi]);
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

	return ret;
}