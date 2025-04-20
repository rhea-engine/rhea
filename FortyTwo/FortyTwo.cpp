#include "World.h"
#include "FortyTwo.h"

#include <string>
#include <Psio/Log.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Avatar.h>
#include <FortyTwo/Accounts/AccountService.h>
#include "Authorization/AuthorizationService.h"
#include <Rhea/Fabrics/Network/ServerService.h>
#include "ConnectionHandler.h"
#include "AuthHandler.h"

void FortyTwo::init()
{
	Rhea::Application::init();

	// ignore controller and spectator defs
	SimpleParser::Type::registerType<Rhea::Unit>("//meworla/controller");
	SimpleParser::Type::registerType<Rhea::Unit>("//meworla/spectator");

	// user and avatar types
	AccountService::RegisterFabrics(this);
	AuthorizationService::RegisterFabrics(this);
	SimpleParser::Type::registerType<Rhea::Avatar>("//meworla/avatar");

	// add world server item
	SimpleParser::Type::registerType<World>("//meworla/world");

	// setup network server
	SimpleParser::Type::registerType<Rhea::ServerService>("//meworla/serverservice");

}

void FortyTwo::setServer(std::string scene)
{
	m_server_name = scene;
}

void FortyTwo::setPort(std::string port)
{
	uint16_t port_i = (uint16_t)atoi(port.c_str());
	if (port_i) {
		m_port = port_i;
	}
}


void FortyTwo::setup()
{
	Rhea::Application::setup();

	if (!m_server_name.length()) {
		LOG(LOG_ERROR) << "No scene defined. Exiting.";
		exit(1);
	}

	// load the world server definition
	loadResource(m_server_name, "//meworla/scene", [&](Rhea::Unit* scene_unit, ReqError err) {
		Rhea::Scene* world_scene = doCast<Rhea::Scene>(scene_unit);
		if (world_scene && err == ReqError::OK) {

			// get the server service
			m_server_service = world_scene->findByType<Rhea::ServerService>();
			if (!m_server_service) {

				/* TODO: if not defined in config add one?
				m_server_service = (ServerService*)SimpleParser::Type::create("ServerService", "//meworla/serverservice");
				m_services["server"] = m_server_service;
				m_server_service->setup(this);

				m_server_service->setNetwork(static_cast<Rhea::PlatformService*>(m_services["platform"])->getEngine()->getNet());
				*/
				LOG(LOG_FATAL) << "no server service defined";
				exit(1);
			}

			// Check setup
			// not configured services will use basic internal version

			// get the account service
			AccountService* account_service = world_scene->findByType<AccountService>();
			if (!account_service) {
				account_service = (AccountService*)SimpleParser::Type::create("AccountService", "//meworla/accountservice");
				m_services["accounts"] = account_service;
				std::unordered_map<std::string, std::string> attributes;
				std::unordered_map<std::string, Variant> values;
				values["provider"] = "memory";
				account_service->setup(this, attributes, values);
				LOG(LOG_INFO) << "No account service defined. Using memory provider.";
			}
			else {
				m_services["accounts"] = account_service;
			}

			World* world = world_scene->findByType<World>();
			if (!world) {
				LOG(LOG_FATAL) << "world not defined";
				exit(1);
			}
			Rhea::Scene* scene = world->getScene();
			if (scene) {
				m_server_service->setPort(m_port);
				m_server_service->setScene(scene);
				m_services["server"] = m_server_service;
				m_server_service->onConnection([&, scene](Rhea::ClientConnection* connection) {
					LOG(LOG_INFO) << "client connected";
					//  add protocols
					std::unique_ptr<ConnectionHandler> data = std::make_unique<ConnectionHandler>(this, scene);
					data->Initialize(connection);
					connection->setConnectionData(std::move(data));
					});
				m_server_service->ready();
				LOG(LOG_INFO) << "server started.";
			}
			else {
				LOG(LOG_FATAL) << "scene not found";
				exit(1);
			}

			// set physics cache
			m_physics = (Rhea::PhysicsService*)m_services["physics"];
		}
		else {
			LOG(LOG_FATAL) << "FortyTwo::setup: could not load scene " << m_server_name;
			exit(1);
		}
		});

}

bool FortyTwo::onFrameEnd()
{
	if (m_server_service)
		return m_server_service->onFrameEnd();
	else
		return false;
}

void FortyTwo::onScene(Rhea::Scene* scene)
{
}

int main(int argc, char* argv[])
{
	FortyTwo* f42 = new FortyTwo();
	f42->setAppname("FortyTwo");
	int argi = 1;

	f42->init();

	while (argi < argc) {
		if (!strcmp(argv[argi], "-key")) {
			argi++;
			//set the ssl key
		}
		else if (!strcmp(argv[argi], "-d")) {
			argi++;
			f42->setAppDir(argv[argi]);
		}
		else if (!strcmp(argv[argi], "-p")) {
			argi++;
			f42->setPort(argv[argi]);
		}
		else if (!strcmp(argv[argi], "-log")) {
			argi++;
			Log::logger().setLogFile(argv[argi]);
		}
		else {
			f42->setServer(argv[argi]);
		}
		argi++;
	}

	f42->setup();

	f42->loop();

	f42->close();
}
