#include "Poseidon.h"
#include <Rhea/Service.h>
#include <Rhea/Graphics/GraphicsManager.h>
#include <Rhea/Graphics/GraphicsService.h>
#include <Rhea/Core/Controller.h>
#include <Psio/Log.h>
#include <Rhea/Physics/Bullet/BtPhysicsService.h>
#include <Rhea/Fabrics/Network/NetworkFabrics.h>
#include <Rhea/Platform/PlatformService.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Resources/ResourceList.h>
#include <Rhea/Resources/ResourceService.h>
#include <Rhea/Graphics/GraphicsComponent.h>

using Rhea::GraphicsService;
using Rhea::Service;

namespace Poseidon
{
	void Application::init()
	{
		Rhea::Application::init();

		SimpleParser::Type::registerType<Rhea::Controller>("//meworla/icontroller");
		SimpleParser::Type::registerType<Rhea::Unit>("//meworla/avatarlist");
	
		GraphicsService* g_service = static_cast<Rhea::GraphicsService*>(getService("graphics"));
		if (g_service) {
			core = g_service->getEngine();
			core->setAppname(m_appname);
		}

		// register network fabrics
		Rhea::NetworkFabrics::Register(this);

		// create a representation for the engine
		std::unordered_map<std::string, std::string> attributes;
		std::unordered_map<std::string, Variant> values;
		
		Rhea::Unit* engine = m_unit_tree->createUnit("engine", "//core/types/unit", attributes, values);
		m_unit_tree->addChild(engine);		
	}

	void Application::setup()
	{
		Rhea::Application::setup();

		// load core resources
		Rhea::ResourceList res_list(this);

		// load shaders
		res_list.push("standard_shader", "//rhea/shaders/standard.shader", "//meworla/shader");
		res_list.push("widget_shader", "//rhea/shaders/widget.shader", "//meworla/shader");
		res_list.push("text_shader", "//rhea/shaders/text.shader", "//meworla/shader");
		res_list.push("terrain_shader", "//rhea/shaders/terrain.shader", "//meworla/shader");
		res_list.push("skeletal_shader", "//rhea/shaders/skeletal.shader", "//meworla/shader");

		// skybox		
		Hippios::DepthStencilStateDesc dss;
		ZeroMemory(&dss, sizeof(dss));
		dss.depth_enable = true;
		dss.depth_write_mask = Hippios::DepthWriteMask::DEPTH_WRITE_ALL;
		dss.depth_func = Hippios::ComparisonFunction::COMPARE_LESS_EQUAL;
		doCast<GraphicsService>(getService("graphics"))->getEngine()->get_renderer()->createStencilState("skybox", &dss);
		res_list.push("skybox_shader", "//rhea/shaders/skybox.shader", "//meworla/shader");

		// note: for locally installed resources this is actually synchronous
		static_cast<Rhea::ResourceService*>(getService("resources"))->loadAsync(res_list, nullptr);
	}

	void Application::onScene(Rhea::Scene* scene)
	{
		log("scene displayed");
		std::unique_ptr<Rhea::Event> ev_r = std::make_unique<Rhea::Event>("ready", scene);
		scene->sendEvent(std::string("ready"), ev_r.get());

		m_phys_debug = scene->getValue("debug_physics");

		if (m_phys_debug) {
			Rhea::BulletPhysicsService* ps = doCast<Rhea::BulletPhysicsService>(getService("physics"));
			if (ps)
				m_debug_draw = new Rhea::BulletDebugDraw(this, ps->getWorld(), core);
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