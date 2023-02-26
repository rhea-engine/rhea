#include "World.h"
#include "FortyTwo.h"

#include <string>
#include <Psio/Log.h>
#include <Rhea/UnitTree.h>

void FortyTwo::init()
{
	Rhea::Application::init();

	// ignore controller and spectator defs
	SimpleParser::Type::registerType<Rhea::Unit>("//meworla/controller");
	SimpleParser::Type::registerType<Rhea::Unit>("//meworla/spectator");

	// add world server item
	SimpleParser::Type::registerType<World>("//meworla/world");

	// setup network server
	SimpleParser::Type::registerType<ServerService>("//meworla/serverservice");

}

void FortyTwo::setServer(std::string scene)
{
	m_server_name = scene;
}

void FortyTwo::setup()
{
	Rhea::Application::setup();

	if (!m_server_name.length()) {
		LOG(LOG_ERROR) << "No scene defined. Exiting.";
		exit(1);
	}

	// load the world server definition
	loadScene(m_server_name, [&](Rhea::Scene* world_scene, ReqError err) {
		if (err == ReqError::OK) {

			m_server_service = (ServerService*)world_scene->findByType("//meworla/serverservice");
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
			else {
				Rhea::Scene* scene = ((World*)world_scene->findByType("//meworla/world"))->getScene();
				if (scene) {
					m_server_service->setScene(scene);
					m_services["server"] = m_server_service;
					m_server_service->ready();
					LOG(LOG_INFO) << "server started.";
				}
				else {
					LOG(LOG_FATAL) << "scene not found";
					exit(1);
				}
			}
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
		else {
			f42->setServer(argv[argi]);
		}
		argi++;
	}

	f42->setup();

	f42->loop();

	f42->close();
}
