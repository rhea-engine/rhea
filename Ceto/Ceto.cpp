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
#include <Psio/Keyboard.h>
#include <Rhea/EventInput.h>
#include <Rhea/Fabrics/GUI/List.h>
#include <Orion/FileStream.h>
#include <Rhea/Fabrics/Core/Avatar.h>

bool Ceto::onQuitDialog(Rhea::Event* ev, Rhea::Unit* unit)
{
	Rhea::Widget* w = static_cast<Rhea::Widget*>(m_unit_tree->find("//local/cetogui/gui/quit"));
	if (w) {
		w->hide();
	}
	ev->stopPropagation();

	// handle the buttons
	Unit* target = ev->getTarget();
	if (!target->getPathName().compare("ok")) {
		// quit the application
		exit(0);
	}

	return false;
}

bool Ceto::onInput(Rhea::EventInput* e, Rhea::Unit* unit)
{
	std::shared_ptr<Psio::InputEvent> evt = e->getEvent();

	// close on esc
	Psio::KeyInputEvent esc(Psio::InputEventId::KEY_PRESSED);
	esc.set(Psio::Key::CODE_ESC, 0, 0);

	if (evt->isEvent(&esc)) {
		Rhea::Widget* w = static_cast<Rhea::Widget*>(m_unit_tree->find("//local/cetogui/gui/quit"));
		if (w) {
			w->show();
		}
		return false;
	}

	// open
	esc.set(Psio::Key::CODE_O, 0, 0);
	if (evt->isEvent(&esc)) {
		Rhea::Widget* w = static_cast<Rhea::Widget*>(m_unit_tree->find("//local/cetogui/gui/realms"));
		if (w) {
			w->show();
		}
		return false;
	}

	return false;
}

bool Ceto::onRealmSelect(Rhea::Event* ev, Rhea::Unit* unit)
{
	Rhea::Widget* w = static_cast<Rhea::Widget*>(m_unit_tree->find("//local/cetogui/gui/realms"));
	if (w) {
		w->hide();
	}
	ev->stopPropagation();

	Unit* target = ev->getTarget();
	DLOG(LOG_DEBUG) << "target selected was '" << (std::string)target->getValue("value") << "'";
	m_home_scene = (std::string)target->getValue("value");

	// open the scene
	setScene();
	SimpleParser::CssParser::PropertyMap properties;
	properties["background-image"] = { "none" };
	w = static_cast<Rhea::Widget*>(m_unit_tree->find("//local/cetogui/gui"));
	w->setStyle(properties);

	return false;
}

void Ceto::setRealmSelectDialog()
{
	// fill last used list
	Rhea::ListWidget* list = static_cast<Rhea::ListWidget*>(m_unit_tree->find("//local/cetogui/gui/realms/lastused"));
	std::vector<std::string> lastused = m_userprefs.lastUsed();
	if (list && lastused.size()) {
		for (auto it = lastused.begin(); it != lastused.end(); it++) {
			list->addItem((*it), (*it));
		}
	}

	// show dialog
	Rhea::Widget* w = static_cast<Rhea::Widget*>(m_unit_tree->find("//local/cetogui/gui/realms"));
	if (w) {
		w->registerEventHandler("selected", this, std::bind(&Ceto::onRealmSelect, this, std::placeholders::_1, std::placeholders::_2));
		w->show();
	}
}

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

	// read from userprefs
	m_userpref_path = m_fs->getFs()->getHomeDir();
	m_userpref_path += string("/ceto.json");

	if (m_fs->getFs()->fileExists(m_userpref_path))
	{
		// file exists		
		std::unique_ptr<Orion::FileResourceStream> stream = std::make_unique<Orion::FileResourceStream>(m_userpref_path);
		std::unique_ptr<Psio::IOBuffer> buf = std::make_unique<Psio::IOBuffer>(64 * 1024);
		if (stream->Read(buf.get())) {
			const char* ptr = buf->reader();
			size_t len = buf->Remaining();		
			json prefs = json::parse(ptr, ptr + len, nullptr, false);
			m_userprefs = prefs;			
		}
	}

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
}

void Ceto::setScene()
{
	m_userprefs.lastUsed(m_home_scene);

	std::string s = m_userprefs.toJSON();
	std::unique_ptr<Orion::FileResourceStream> stream = std::make_unique<Orion::FileResourceStream>(m_userpref_path);
	stream->SetMode(Orion::Fs::WRITE);
	stream->Open();
	stream->Write(s.c_str(), s.size());
	stream->Close();		

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
						client->setAuth("anonymous");
						client->start(scene, 
							[&, client](int err) {
								if (!err) {
									showAvatarSelect(scene, client);
								}
								else {
									switch (err) {
									case -2:										
										showAlert("Avatar not found");
										break;
									default:
										showAlert("General connection error");
									}
								}
							},
							[&, scene](int err) {
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
	std::unique_ptr<Rhea::Event> ev = std::make_unique<Rhea::Event>("display", scene);
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
		// store in local "cache" var
		m_physics = physics;
	}

	// search for a camera			
	m_view = static_cast<Rhea::Camera*>(scene->findByType("//meworla/camera"));
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
}

void Ceto::showAvatarSelect(Rhea::Scene* scene, Rhea::Unit* client)
{
	// fill last used list	
	Rhea::ListWidget* list = static_cast<Rhea::ListWidget*>(m_unit_tree->find("//local/cetogui/gui/avatars/list"));	
	if (list) {
		Rhea::Unit* avatar_list = scene->getChild("avatarlist");
		if (avatar_list) {
			avatar_list->children([&list](std::string& name, Rhea::Unit* unit)->bool {
				if (unit->getTypeName() == "//meworla/avatar") {
					Rhea::Avatar* avatar = (Rhea::Avatar*)unit;
					list->addItem(name, name);
				}
			return true;
				});
		}
	}

	// show dialog
	Rhea::Widget* w = static_cast<Rhea::Widget*>(m_unit_tree->find("//local/cetogui/gui/avatars"));
	if (w) {
		w->setUserPtr((void*)client);
		w->registerEventHandler("selected", this, std::bind(&Ceto::onAvatarSelect, this, std::placeholders::_1, std::placeholders::_2));
		w->show();
	}
}

bool Ceto::onAvatarSelect(Rhea::Event* ev, Rhea::Unit* unit)
{
	Rhea::Widget* w = static_cast<Rhea::Widget*>(m_unit_tree->find("//local/cetogui/gui/avatars"));
	if (w) {
		w->hide();
	}
	ev->stopPropagation();

	Unit* target = ev->getTarget();
	DLOG(LOG_DEBUG) << "target selected was '" << (std::string)target->getValue("value") << "'";
	std::string avatar_name = (std::string)target->getValue("value");

	// enter the world with given avatarname
	if (avatar_name.length()) {
		Poseidon::WorldClient* client = (Poseidon::WorldClient*)w->getUserPtr();
		client->enterWorld(avatar_name);
	//	SimpleParser::CssParser::PropertyMap properties;
	//	properties["background-image"] = { "none" };
	//	w = static_cast<Rhea::Widget*>(m_unit_tree->find("//local/cetogui/gui"));
	//	w->setStyle(properties);
	}

	return false;
}

void Ceto::showAlert(std::string text)
{
	// set the text
	Rhea::Label* info_text = static_cast<Rhea::Label*>(m_unit_tree->find("//local/cetogui/gui/alert/info"));
	if (info_text) {
		info_text->setText(text);
	}
	 
	// show dialog
	Rhea::Widget* w = static_cast<Rhea::Widget*>(m_unit_tree->find("//local/cetogui/gui/alert"));
	if (w) {		
		w->registerEventHandler("clicked", this, std::bind(&Ceto::onAlertDialog, this, std::placeholders::_1, std::placeholders::_2));
		w->show();
	}
}

bool Ceto::onAlertDialog(Rhea::Event* ev, Rhea::Unit* unit)
{
	Rhea::Widget* w = static_cast<Rhea::Widget*>(m_unit_tree->find("//local/cetogui/gui/alert"));
	if (w) {
		w->hide();
	}
	ev->stopPropagation();

	return false;
}

void Ceto::loadMainGUI()
{
	// load the apps gui			
	std::string app_gui = "//local/cetogui.scene";
	loadResource(app_gui, "//meworla/gui", [&](Rhea::Unit* gui, ReqError r_err) {
		if (r_err == ReqError::OK) {
			
			gui->ready();
			log("gui ready");
			static_cast<Rhea::GraphicsService*>(m_services["graphics"])->registerScene("HUD", gui);

			// find label for debug text
			m_debug_text = static_cast<Rhea::Label*>(m_unit_tree->find("//local/cetogui/gui/widget1/laber"));
			if (!m_debug_text) {
				log("no debug text label found");
			}

			// setup system dialogs

			// quit dialog
			Rhea::Widget* w = static_cast<Rhea::Widget*>(m_unit_tree->find("//local/cetogui/gui/quit"));
			if (w) {
				w->registerEventHandler("clicked", this, std::bind(&Ceto::onQuitDialog, this, std::placeholders::_1, std::placeholders::_2));
			}

			// realm selection
			setRealmSelectDialog();
		}
		else {
			log("loading of gui failed");
		}
		});
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

	app->loadMainGUI();
		
	//app->setScene();

	app->loop();	

	app->close();

	return 0;
}