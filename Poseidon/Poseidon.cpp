#include "Poseidon.h"
#include <Rhea/Service.h>
#include <Rhea/Graphics/GraphicsManager.h>
#include <Rhea/Graphics/GraphicsService.h>
#include <Rhea/Controller/Controller.h>
#include <Rhea/Controller/BasicController.h>
#include <Rhea/Controller/SpectatorController.h>
#include <Psio/Log.h>
#include <Rhea/Physics/Bullet/BtPhysicsService.h>
#include <Network/ClientService.h>
#include <Rhea/Platform/PlatformService.h>

using Rhea::GraphicsService;
using Rhea::Service;
using Rhea::BasicController;
using Rhea::SpectatorController;

namespace Poseidon
{
	void Application::init()
	{
		Rhea::Application::init();

		// register services
		SimpleParser::Type::registerType<GraphicsService>("//meworla/graphicsservice");


		SimpleParser::Type::registerType<BasicController>("//meworla/controller");
		SimpleParser::Type::registerType<SpectatorController>("//meworla/spectator");
		SimpleParser::Type::registerType<ClientService>("//meworla/clientservice");
		SimpleParser::Type::registerType<WorldClient>("//meworla/worldclient");
	
		GraphicsService* g_service = (GraphicsService*)SimpleParser::Type::create("GraphicsService", "//meworla/graphicsservice");
		g_service->setup(this);
		m_services["graphics"] = g_service;

		Rhea::PlatformService* psio = static_cast<Rhea::PlatformService *>(m_services["platform"]);
		g_service->setPlatform(psio);

		ClientService* client = (ClientService*)SimpleParser::Type::create("ClientService", "//meworla/clientservice");
		client->setNetwork(psio->getEngine()->getNet());
		client->setup(this);
		m_services["clientservice"] = client;

		core = g_service->getEngine();
	}

	void Application::onScene(Rhea::Scene* scene)
	{
		log("scene displayed");
		std::unique_ptr<Rhea::Event> ev_r = std::make_unique<Rhea::Event>("ready", scene);
		scene->sendEvent(std::string("ready"), ev_r.get());

		m_phys_debug = scene->getValue("debug_physics");

		if (m_phys_debug) {
			Rhea::BulletPhysicsService* ps = (Rhea::BulletPhysicsService*)getService("physics");
			if (ps)
				m_debug_draw = new Rhea::BulletDebugDraw(this, ps->getWorld());
		}
	}

	void Application::toggleDebug()
	{
		if (m_debug_draw)
			m_debug_draw->toggle();
		if (!m_phys_debug)
			LOG(LOG_ERROR) << "physics debug drawer was not enabled at startup";
	}
}