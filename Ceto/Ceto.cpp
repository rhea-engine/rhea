#include "Ceto.h"
#include "Rhea/Scene.h"
#include "Rhea/Unit.h"
#include "Rhea/UnitTree.h"
#include "Rhea/Controller/BasicController.h"
#include "Rhea/Controller/SpectatorController.h"
#include "Rhea/Graphics/GraphicsService.h"
#include "Rhea/Graphics/Camera.h"
#include "Rhea/Camera.h"
#include "Rhea/Physics/PhysicsManager.h"

void Ceto::setup()
{
	Rhea::Application::setup();

	// check for config file

	// load home scene if any
	if (!m_home_scene.length())
	{
		m_home_scene = "//Ceto/Init/init.scene";		
	}
	loadScene(m_home_scene, [&](Rhea::Scene* scene, ReqError err) {
		if (err == ReqError::OK) {
			static_cast<Rhea::GraphicsService*>(m_services["graphics"])->setScene(scene);
			std::unique_ptr<Rhea::Event> ev = std::make_unique<Rhea::Event>("display");
			scene->sendEvent(std::string("display"), ev.get());
			scene->broadcast("show");
			Rhea::PhysicsService* physics = static_cast<Rhea::PhysicsService*>(m_services["physics"]);
			if (!physics) {
				Variant phy = scene->getValue("physics");
				if (phy == std::string("none")) {
					physics = static_cast<Rhea::PhysicsManagerService*>(m_services["physics_mgr"])->getPhysicsService(Rhea::PhysicsManagerService::Types::NONE);
				}
				else {
					physics = static_cast<Rhea::PhysicsManagerService*>(m_services["physics_mgr"])->getPhysicsService(Rhea::PhysicsManagerService::Types::WORLD);
				}
				physics->onInit();
				m_services["physics"] = physics;
			}

			// search for a controller
			m_focused_controller = static_cast<Rhea::Controller*>(m_unit_tree->findByType("//meworla/controller"));
			if (!m_focused_controller) {
				// no controller found - search for camera and add default controller for it
				// note: spectator does not work, because we have no controlled element
				m_view = static_cast<Rhea::Camera*>(m_unit_tree->findByType("//meworla/camera"));
				if (!m_view) {
					// no camera found - create a default cam
					m_view = static_cast<Rhea::Camera*>(m_unit_tree->createUnitById(scene->getPath() + "/camera", "//meworla/camera"));
					Rhea::GraphicsCamera::createElement(core, m_view);
					m_view->setPosition(Poseidon::Vector3(0, 1.7f, 0.0f));
				}
				m_focused_controller = new Rhea::BasicController("camera_controller", "//meworla/controller");
				m_focused_controller->connect(m_view);
				scene->addChild(m_focused_controller);
			}
			static_cast<Rhea::PhysicsService*>(m_services["physics"])->setScene(static_cast<Rhea::Scene*>(scene));

			// now we are ready
			log("scene displayed");
			std::unique_ptr<Rhea::Event> ev_r = std::make_unique<Rhea::Event>("ready");
			scene->sendEvent(std::string("ready"), ev_r.get());

			//m_unit_tree->broadcast("logme");
		}
		else {
			log("Could not open scene");
		}
		});
}

void Ceto::setHomeScene(string scene)
{
	m_home_scene = scene;
}

int main(int argc, char* argv[])
{
	Ceto* app = new Ceto();
	app->setAppname("Ceto");
	int argi = 1;

	app->init();

	while (argi < argc) {
		if (!strcmp(argv[argi], "-d")) {
			argi++;
			app->setAppDir(argv[argi]);
		}
		else
			app->setHomeScene(argv[argi]);
		argi++;
	}
		
	app->setup();

	app->loop();	

	app->close();

	return 0;
}