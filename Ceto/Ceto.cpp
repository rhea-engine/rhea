#include "Ceto.h"
#include "Rhea/Fabrics/Core/Scene.h"
#include "Rhea/Unit.h"
#include "Rhea/UnitTree.h"
#include "Rhea/Controller/BasicController.h"
#include "Rhea/Controller/SpectatorController.h"
#include "Rhea/Graphics/GraphicsService.h"
#include "Rhea/Graphics/CameraManager.h"
#include "Rhea/Fabrics/3D/Camera.h"
#include "Rhea/Physics/PhysicsManager.h"
#include "Rhea/Graphics/GraphicsService.h"
#include "Rhea/Graphics/GraphicsManager.h"

#include "Poseidon/Material.h"
#include "Poseidon/Shader.h"
#include "Rhea/Fabrics/Core/Material.h"
#include <Poseidon/Network/ClientService.h>
#include <Psio/Log.h>

void Ceto::setup()
{
	Poseidon::Application::setup();

	// check for config file

	// load home scene if any
	if (!m_home_scene.length())
	{
		m_home_scene = "//Ceto/Init/init.scene";		
	}

	// add defaults

	// a color material
	Poseidon::Material* material = get_core()->getMaterial("color");
	if (!material) {
		material = get_core()->createMaterial();
		material->setTopology(2);
		Poseidon::Shader* shader = get_core()->getShader("shader_vp");
		if (!shader) {
			shader = get_core()->createShader("shader_vp");
			shader->setVertexType(0);
			if (shader->loadFromFile("Data\\Rhea\\shaders\\shader_vp.hlsl", "VShader", "PShader"))
				exit(-1);
		}
		material->setShader(shader);
		get_core()->registerMaterial("color", material);
	}
	Rhea::Material* mat = new Rhea::Material("color");
	mat->setup(this);	
	mat->setValue("shader", "shader_vp");
	m_unit_tree->addChild(mat);

	loadScene(m_home_scene, [&](Rhea::Scene* scene, ReqError err) {
		if (err == ReqError::OK) {

			// is this network controlled?
			Variant worldserver = scene->getValue("worldserver");
			if (worldserver != Variant()) {
				Poseidon::ClientService* client_service = (Poseidon::ClientService*)getService("clientservice");
				client_service->connect(worldserver, [&, scene](Poseidon::WorldClient* client, int err) {
					if (!err)
					{
						scene->addChild(client);
						DLOG(LOG_DEBUG) << "connected to server";
						client->setAuth("gnar");
						client->start(scene, [&, scene](int err) {
							if (!err) {
								showScene(scene);
							}
							});
					}
					else {
						
						// TODO show choose realm screen
						// for now exit
						LOG(LOG_FATAL) << "Could not connect to server";
						exit(-1);
					}
					});
			}
			else {
				// no special handling - display scene
				showScene(scene);
			}
		}
		else {
			log("Could not open scene");
		}
		});
}

void Ceto::showScene(Rhea::Scene* scene)
{
	static_cast<Rhea::GraphicsService*>(m_services["graphics"])->setScene(scene);
	std::unique_ptr<Rhea::Event> ev = std::make_unique<Rhea::Event>("display");
	scene->sendEvent(std::string("display"), ev.get());
	scene->broadcast("show");

	// initialize physics
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

	// search for a camera			
	m_view = static_cast<Rhea::Camera*>(m_unit_tree->findByType("//meworla/camera"));
	if (!m_view) {
		// no camera found - create a default cam
		m_view = static_cast<Rhea::Camera*>(m_unit_tree->createUnitById(scene->getPath() + "/camera", "//meworla/camera"));

		Rhea::GraphicsService* gs = static_cast<Rhea::GraphicsService*>(getService("graphics"));
		Rhea::GraphicsManager* mgr = gs->getManager("//meworla/camera");
		//Rhea::GraphicsCamera::createElement(core, m_view);
		mgr->createElement(core, m_view);
		Variant pos = scene->getValue("start");
		if (pos != Variant()) {
			Math::Vector3 start_pos(pos[0], pos[1], pos[2]);
			m_view->setPosition(start_pos);
		}
		else {
			m_view->setPosition(Math::Vector3(0, 1.7f, 0.0f));
		}
	}

	// search for a controller
	m_focused_controller = static_cast<Rhea::Controller*>(m_unit_tree->findByType("//meworla/controller"));
	if (!m_focused_controller) {
		// no controller found - use camera and add default controller for it
		// note: spectator does not work, because we have no controlled element
		m_focused_controller = new Rhea::BasicController("camera_controller", "//meworla/controller");
		m_focused_controller->setup(this);
		m_focused_controller->connect(m_view);
		scene->addChild(m_focused_controller);
	}
	static_cast<Rhea::PhysicsService*>(m_services["physics"])->setScene(static_cast<Rhea::Scene*>(scene));

	// now we are ready
	onScene(scene);

	// load the apps gui			
	std::string app_gui = "//local/cetogui.scene";
	loadResource(app_gui, "//meworla/gui", [&](Rhea::Unit* gui, ReqError r_err) {
		if (r_err == ReqError::OK) {
			gui->prepare([&](Rhea::Error* err) {
				gui->ready();
			log("gui ready");

			// find label for debug text
			m_debug_text = static_cast<Rhea::Label*>(m_unit_tree->find("//local/cetogui/gui/widget1/laber"));
			if (!m_debug_text) {
				log("no debug text label found");
			}
				});
		}
		else {
			log("loading of gui failed");
		}
		});

	//m_unit_tree->broadcast("logme");
}

void Ceto::setHomeScene(string scene)
{
	m_home_scene = scene;
}

bool Ceto::onFrameEnd()
{
	static unsigned int show_ticks = 0;

	if (m_debug_text) {
		unsigned int ticks_s = (unsigned int)(current_ticks / 1000000L);
		if (ticks_s > show_ticks) {
			show_ticks = ticks_s;
			std::string debug = "";
			debug += "debugging: s=" + std::to_string(show_ticks) + "; fps=" + std::to_string(fps);
			if (m_focused_controller) {
				Math::Vector3 pos = static_cast<Rhea::BasicController*>(m_focused_controller)->getPosition();
				debug += "\nPosition: " + std::to_string(pos.x) + " / " + std::to_string(pos.y) + " / " + std::to_string(pos.z);
			}
			m_debug_text->setText(debug);
		}		
	}

	return false;
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