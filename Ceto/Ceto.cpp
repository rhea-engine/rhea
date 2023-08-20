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
#include <Rhea/Fabrics/Network/ClientService.h>
#include <Psio/Log.h>
#include <Psio/Keyboard.h>
#include <Rhea/EventInput.h>
#include <Rhea/Fabrics/GUI/List.h>
#include <Orion/FileStream.h>
#include <Rhea/Fabrics/Core/Avatar.h>
#include <Rhea/Resources/Collection.h>

bool Ceto::onQuit(Rhea::Event* ev, Rhea::Unit* unit)
{
	ev->stopPropagation();

	exit(0);

	return false;
}

bool Ceto::onInput(Rhea::EventInput* e, Rhea::Unit* unit)
{
	std::shared_ptr<Psio::InputEvent> evt = e->getEvent();

	// close on esc
	Psio::KeyInputEvent esc(Psio::InputEventId::KEY_PRESSED);
	esc.set(Psio::Key::CODE_ESC, 0, 0);

	if (evt->isEvent(&esc)) {
		Rhea::Widget* w = doCast<Rhea::Widget>(m_unit_tree->find("//local/cetogui/gui/quit"));
		if (w) {
			w->show();
			w = doCast<Rhea::Widget>(m_unit_tree->find("//local/cetogui/gui/realms"));
			if (w) {
				w->hide();
			}
		}
		return false;
	}

	// open
	esc.set(Psio::Key::CODE_O, 0, 0);
	if (evt->isEvent(&esc)) {
		Rhea::Widget* w = doCast<Rhea::Widget>(m_unit_tree->find("//local/cetogui/gui/realms"));
		if (w) {
			w->show();
		}
		return false;
	}

	return false;
}

bool Ceto::onRealmSelect(Rhea::Event* ev, Rhea::Unit* unit)
{
	ev->stopPropagation();

	Unit* target = ev->getTarget();
	DLOG(LOG_DEBUG) << "target selected was '" << (std::string)target->getValue("value") << "'";

	openScene((std::string)target->getValue("value"));
	return false;
}

void Ceto::setRealmSelectDialog()
{
	// show dialog
	Rhea::Widget* w = doCast<Rhea::Widget>(m_unit_tree->find("//local/cetogui/gui/realms"));
	if (w) {
		w->show();
	}
}

void Ceto::openScene(std::string scene_name)
{
	Rhea::Widget* w = doCast<Rhea::Widget>(m_unit_tree->find("//local/cetogui/gui/realms"));
	if (w) {
		w->hide();
	}
	m_home_scene = scene_name;

	// open the scene
	setScene();	
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
	m_userpref_path = m_fs->getFs()->getAppDataDir();
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

	std::unordered_map<std::string, std::string> attributes;
	std::unordered_map<std::string, Variant> values;

	// create a representation for the engine
	Unit* engine = m_unit_tree->createUnit("engine", "//core/types/unit", attributes, values);
	m_unit_tree->addChild(engine);

	// create a user unit

	std::vector<std::string> lastused = m_userprefs.lastUsed();
	if (lastused.size()) {
		std::vector<Variant> lastused_arr;
		for (auto it = lastused.begin(); it != lastused.end(); it++) {
			lastused_arr.push_back(*it);
		}
		values["lastused"] = lastused_arr;
	}
	Unit* user = m_unit_tree->createUnit("user", "//core/types/unit", attributes, values);
	engine->addChild(user);

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

void Ceto::setScene(bool mark_preferrences)
{
	if (mark_preferrences) {
		m_userprefs.lastUsed(m_home_scene);

		std::string s = m_userprefs.toJSON();
		std::unique_ptr<Orion::FileResourceStream> stream = std::make_unique<Orion::FileResourceStream>(m_userpref_path);
		stream->SetMode(Orion::Fs::WRITE);
		stream->Open();
		stream->Write(s.c_str(), s.size());
		stream->Close();
	}

	loadResource(m_home_scene, "//meworla/scene", [&](Rhea::Unit* scene_unit, ReqError err) {
		Rhea::Scene* scene = doCast<Rhea::Scene>(scene_unit);
		if (scene && err == ReqError::OK) {

			// hide GUI background
			SimpleParser::CssParser::PropertyMap properties;
			properties["background-image"] = { "none" };
			Rhea::Widget* w = doCast<Rhea::Widget>(m_unit_tree->find("//local/cetogui/gui"));
			if (w)
				w->setStyle(properties);

			// display scene
			std::string start_scene = scene->getValue("startscene", "");
			if (start_scene != "") {
				Rhea::Scene* go_scene = doCast<Rhea::Scene>(m_unit_tree->find(start_scene, scene->getPath(), false));
				if (go_scene)
					showScene(go_scene);
				else {
					LOG(LOG_ERROR) << "Startscene '" << start_scene << "' not found.";
					showScene(scene);
				}
			}
			else {
				showScene(scene);
			}
		}
		else {
			Rhea::Collection* dir = doCast<Rhea::Collection>(scene_unit);
			if (dir) {
				// user tried to open directory
				// try world.scene
				m_home_scene += "/world.scene";
				return setScene(false);
			}

			log("Could not open scene");
			Rhea::Widget* w = doCast<Rhea::Widget>(m_unit_tree->find("//local/cetogui/gui/realms"));
			if (w) {
				w->hide();
			}
			showAlert("Could not open scene");
		}
		});
}

void Ceto::showScene(Rhea::Scene* scene)
{
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
	m_view = scene->findByType<Rhea::Camera>();
	if (!m_view) {
		// no camera found - create a default cam
		m_view = static_cast<Rhea::Camera*>(m_unit_tree->createUnit("camera", "//meworla/camera"));
		scene->addChild(m_view);

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
	m_focused_controller = m_unit_tree->findByType<Rhea::Controller>();
	if (!m_focused_controller) {
		// no controller found - use camera and add default controller for it
		// note: spectator does not work, because we have no controlled element
		m_focused_controller = new Rhea::BasicController("camera_controller", "//meworla/controller");
		m_focused_controller->setup(this);
		m_focused_controller->connect(m_view);
		scene->addChild(m_focused_controller);
	}
	// now we are ready
	scene->show();
}

void Ceto::showAlert(std::string text)
{
	// set the text
	m_gui->setValue("alert", text);
	 
	// show dialog
	Rhea::Widget* w = doCast<Rhea::Widget>(m_unit_tree->find("//local/cetogui/gui/alert"));
	if (w) {
		w->show();
	}
}

void Ceto::loadMainGUI()
{
	// load the apps gui			
	std::string app_gui = "//local/cetogui.scene";
	loadResource(app_gui, "//meworla/scene", [&](Rhea::Unit* res, ReqError r_err) {
		Rhea::Scene* gui = doCast<Rhea::Scene>(res);
		if (gui && r_err == ReqError::OK) {
			
			gui->show();
			m_gui = gui;
			log("gui ready");
			
			// find label for debug text
			m_debug_text = doCast<Rhea::Label>(m_unit_tree->find("//local/cetogui/gui/widget1/laber"));
			if (!m_debug_text) {
				log("no debug text label found");
			}

			// setup system dialogs

			// quit dialog
			gui->registerEventHandler("quit", this, std::bind(&Ceto::onQuit, this, std::placeholders::_1, std::placeholders::_2));

			// realm selection
			setRealmSelectDialog();

			// register open
			gui->registerEventHandler("open", this, std::bind(&Ceto::onRealmSelect, this, std::placeholders::_1, std::placeholders::_2));
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
	static float elapsed_s = 0.0f;

	if (m_debug_text) {
		float elapsed_d_s = static_cast<float>(elapsed_ticks) / 1000000.0f;
		elapsed_s += elapsed_d_s;

		std::string debug = "";
		debug += "debugging: sec=" + std::to_string(elapsed_s) +"; fps = " + std::to_string(fps);
		m_focused_controller = doCast<Rhea::BasicController>(m_unit_tree->find("//local/scene1/maincamera"));
		if (m_focused_controller) {
			Math::Vector3 pos = static_cast<Rhea::BasicController*>(m_focused_controller)->getPosition();
			debug += "\nPosition: " + std::to_string(pos.x) + " / " + std::to_string(pos.y) + " / " + std::to_string(pos.z);
		}
		m_debug_text->setText(debug);
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
			LOG(LOG_INFO) << "Set //local to directory: " << argv[argi];
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