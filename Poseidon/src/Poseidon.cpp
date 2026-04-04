#include "Poseidon/Poseidon.h"
#include <Rhea/Service.h>
#include <RheaClient/Graphics/GraphicsManager.h>
#include <RheaClient/Graphics/GraphicsService.h>
#include <Rhea/Core/Controller.h>
#include <Psio/Log.h>
#include <Rhea/Platform/PlatformService.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Resources/ResourceList.h>
#include <Rhea/Resources/ResourceService.h>
#include <Rhea/Physics/PhysicsManagerService.h>
#include <Rhea/Physics/IPhysicsDebugDraw.h>
#include <Rhea/Physics/PhysicsService.h>
#include <cstring> // for memset

using rhea::client::GraphicsService;
using rhea::Service;

namespace poseidon
{
	void Application::init(int argc, char* argv[])
	{
		rhea::Application::init(argc, argv);

		simpleparser::Type::registerType<rhea::Controller>("//meworla/icontroller");
		simpleparser::Type::registerType<rhea::Unit>("//meworla/avatarlist");
	
		GraphicsService* g_service = static_cast<rhea::client::GraphicsService*>(getService("graphics"));
		if (g_service) {
			core = g_service->getEngine();
			core->setAppname(m_appname);
		}

		// create a representation for the engine
		std::unordered_map<std::string, std::string> attributes;
		std::unordered_map<std::string, Variant> values;
		
		rhea::Unit* engine = m_unit_tree->createUnit("engine", "//core/types/unit", attributes, values);
		m_unit_tree->addChild(engine);		
	}

	void Application::setup()
	{
		rhea::Application::setup();

		// load core resources
		rhea::ResourceList res_list(this);

		// load shaders
		res_list.push("standard_shader", "//rhea/shaders/standard.shader", "//meworla/shader");
		res_list.push("widget_shader", "//rhea/shaders/widget.shader", "//meworla/shader");
		res_list.push("text_shader", "//rhea/shaders/text.shader", "//meworla/shader");
		res_list.push("terrain_shader", "//rhea/shaders/terrain.shader", "//meworla/shader");
		res_list.push("skeletal_shader", "//rhea/shaders/skeletal.shader", "//meworla/shader");

		// skybox		
		hippios::DepthStencilStateDesc dss;
		std::memset(&dss, 0, sizeof(dss));
		dss.depth_enable = true;
		dss.depth_write_mask = hippios::DepthWriteMask::DEPTH_WRITE_ALL;
		dss.depth_func = hippios::ComparisonFunction::COMPARE_LESS_EQUAL;
		doCast<GraphicsService>(getService("graphics"))->getEngine()->get_renderer()->createStencilState("skybox", dss);
		res_list.push("skybox_shader", "//rhea/shaders/skybox.shader", "//meworla/shader");

		// note: for locally installed resources this is actually synchronous
		static_cast<rhea::ResourceService*>(getService("resources"))->loadAsync(res_list, nullptr);
	}

	void Application::onScene(rhea::Scene* scene)
	{
		log("scene displayed");
		std::unique_ptr<rhea::Event> ev_r = std::make_unique<rhea::Event>("ready", scene);
		scene->sendEvent(std::string("ready"), ev_r.get());

		m_phys_debug = scene->getValue("debug_physics");

		if (m_phys_debug) {

			auto* mgr = doCast<rhea::PhysicsManagerService>(getService("physics_mgr"));
			auto* ps = mgr ? mgr->getPhysicsService() : nullptr;

			auto* m_debug_draw = ps ? ps->debugDraw() : nullptr;			
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