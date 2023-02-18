#include "World.h"
#include <Psio/Log.h>
#include <Rhea/Physics/PhysicsService.h>
#include <Rhea/Physics/PhysicsManager.h>
#include "ServerService.h"
#include <Rhea/Platform/PlatformService.h>

void World::init()
{
	Rhea::Application::init();

	SimpleParser::Type::registerType<Rhea::Unit>("//meworla/controller");
	SimpleParser::Type::registerType<Rhea::Unit>("//meworla/spectator");

	// setup network server
	SimpleParser::Type::registerType<ServerService>("//meworla/serverservice");
	m_server_service = (ServerService*)SimpleParser::Type::create("ServerService", "//meworla/serverservice");
	m_services["server"] = m_server_service;
	m_server_service->setup(this);
	
	m_server_service->setNetwork(static_cast<Rhea::PlatformService*>(m_services["platform"])->getEngine()->getNet());
}

void World::setScene(std::string scene)
{
	m_scene = scene;
}

void World::setup()
{
	Rhea::Application::setup();

	if (!m_scene.length()) {
		LOG(LOG_ERROR) << "No scene defined. Exiting.";
		exit(1);
	}

	loadScene(m_scene, [&](Rhea::Scene* scene, ReqError err) {
		if (err == ReqError::OK) {
			Rhea::PhysicsService* physics = static_cast<Rhea::PhysicsService*>(m_services["physics"]);
			if (!physics) {
				Variant phy = scene->getValue("physics");
				if (phy == std::string("none")) {
					physics = static_cast<Rhea::PhysicsManagerService*>(m_services["physics_mgr"])->createPhysicsService(Rhea::PhysicsManagerService::Types::NONE);
				}
				else {
					physics = static_cast<Rhea::PhysicsManagerService*>(m_services["physics_mgr"])->createPhysicsService(Rhea::PhysicsManagerService::Types::WORLD);
				}
				physics->onInit();
				m_services["physics"] = physics;
			}

			static_cast<Rhea::PhysicsService*>(m_services["physics"])->setScene(static_cast<Rhea::Scene*>(scene));

			// now we are ready
			onScene(scene);

		}
		else {
			LOG(LOG_ERROR) << "Could not load scene";
		}
		});
}

bool World::onFrameEnd()
{
	return m_server_service->onFrameEnd();	
}

void World::onScene(Rhea::Scene* scene)
{
	log("scene displayed");
	std::unique_ptr<Rhea::Event> ev_r = std::make_unique<Rhea::Event>("ready");
	scene->sendEvent(std::string("ready"), ev_r.get());
	m_server_service->setScene(scene);
}
