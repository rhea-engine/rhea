/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Ceto.h"
#include "Rhea/Core/Scene.h"
#include "Rhea/Unit.h"
#include "Rhea/UnitTree.h"
#include "Rhea/Controller/BasicController.h"
#include "Rhea/Controller/SpectatorController.h"
#include <RheaClient/Graphics/GraphicsService.h>
#include <RheaClient/Graphics/GraphicsManager.h>
#include <RheaClient/Graphics/CameraManager.h>
#include "Rhea/Core/Camera.h"
#include "Rhea/Physics/PhysicsManagerService.h"
#include "Rhea/Physics/PhysicsService.h"

#include "Poseidon/Material.h"
#include "Poseidon/Shader.h"
#include "Rhea/Core/Material.h"
#include <Psio/Log.h>
#include <Psio/Input/Keyboard.h>
#include <Rhea/EventInput.h>
#include <Rhea/GUI/List.h>
#include <Orion/FileStream.h>
#include <Rhea/Core/Avatar.h>
#include <Rhea/Resources/Collection.h>
#include <Math/Math.h>
#include <Rhea/Animation/Animation.h>
#include <Rhea/Animation/Animator.h>
#include <Rhea/GUI/GUIController.h>
#include <Rhea/Core/Mesh.h>
#include <Rhea/ConfigFile.h>
#include <Rhea/GUI/Button.h>
#include <Rhea/GUI/Dialog.h>
#include <RheaAudio/AudioService.h>

bool Ceto::onQuit(rhea::Event* ev, rhea::Unit* unit)
{
	ev->stopPropagation();

	// user confirmed quit
	m_exiting = true;

	return true;
}

bool Ceto::onQuitCancel(rhea::Event* ev, rhea::Unit* unit)
{
	ev->stopPropagation();

	// user cancelled quit
	m_gui_controller->closeDialog((rhea::Dialog*)unit);
	// show realms dialog again
	if (!inScene) {
		// if we are not in a scene, show the realms dialog again
		// if we are in a scene, we just stay there and ignore the quit request
		rhea::Widget* w = doCast<rhea::Widget>(m_gui_controller->find("realms"));
		if (w) {
		//	m_gui_controller->openDialog((rhea::Dialog*)w);
		}
		return true;
	}
	return false;
}

bool Ceto::onInput(rhea::EventInput* e, rhea::Unit* unit)
{
	rhea::InputEvent evt = e->getEvent();

	if (handleAppShortcuts(evt))
		return true;

#if defined(CETO_DEV_TOOLS)
	if (handleDevShortcuts(evt))
		return true;
#endif

	return false;
}

bool Ceto::handleAppShortcuts(const rhea::InputEvent& evt)
{
	if (evt.isKeyPressed(rhea::Key::Escape)) {
		openQuitDialog();
		return true;
	}

	if (evt.isKeyPressed(rhea::Key::O)) {
		openRealmDialog();
		return true;
	}

	return false;
}

void Ceto::openQuitDialog()
{
	if (!m_gui_controller)
		return;

	rhea::Widget* w = doCast<rhea::Widget>(m_gui_controller->find("quit"));
	if (w) {
		m_gui_controller->openDialog((rhea::Dialog*)w);
	}
}

void Ceto::openRealmDialog()
{
	if (!m_gui_controller)
		return;

	rhea::Widget* w = doCast<rhea::Widget>(m_gui_controller->find("realms"));
	if (w) {
		m_gui_controller->openDialog((rhea::Dialog*)w);
	}
}

#if defined(CETO_DEV_TOOLS)
bool Ceto::handleDevShortcuts(const rhea::InputEvent& evt)
{
	if (evt.isKeyPressed(rhea::Key::F)) {
		devToggleSceneAnimation();
		return true;
	}

	if (evt.isKeyPressed(rhea::Key::T)) {
		devAddTestLabel();
		return true;
	}

	if (evt.isKeyPressed(rhea::Key::Z)) {
		devAddTestWidget();
		return true;
	}

	if (evt.isKeyPressed(rhea::Key::U)) {
		devAddTestButton();
		return true;
	}

	if (evt.isKeyPressed(rhea::Key::I)) {
		devAddTestDialog();
		return true;
	}

	static rhea::audio::AudioSoundHandle snd_handle;
	static rhea::audio::AudioSoundHandle music_handle;

	if (evt.isKeyPressed(rhea::Key::L)) {
		rhea::audio::AudioService* audio_srv = doCast<rhea::audio::AudioService>(getService("audio"));
		if (audio_srv) {
			rhea::audio::AudioResource* res = doCast<rhea::audio::AudioResource>(m_unit_tree->find("//local/scene1/impact"));
			if (res) {
				snd_handle = audio_srv->play(res, "Master", false, 1.0f);
			}
		}
		return true;
	}
	if (evt.isKeyPressed(rhea::Key::K)) {
		rhea::audio::AudioService* audio_srv = doCast<rhea::audio::AudioService>(getService("audio"));
		if (audio_srv) {
			rhea::audio::AudioResource* res = doCast<rhea::audio::AudioResource>(m_unit_tree->find("//local/scene1/snowfall"));
			if (res) {
				static bool snd_running = false;
				if (!snd_running) {
					music_handle = audio_srv->play(res, "Master", true, 1.0f);
					snd_running = true;
				}
				else {
					audio_srv->stop(music_handle);
					snd_running = false;
				}
			}
		}
		return true;
	}

	if (evt.isKeyPressed(rhea::Key::Num0)) {
		auto* mgr = doCast<rhea::PhysicsManagerService>(getService("physics_mgr"));
		auto* physics_srv = mgr ? mgr->getPhysicsService() : nullptr;
		static rhea::IPhysicsDebugDraw* debug_draw = nullptr;
		if (!debug_draw) {
			debug_draw = physics_srv->getDebugDrawer();
			if (m_current_scene) {
				m_current_scene->addChild(debug_draw);
				debug_draw->setEnabled(true);
			}
		}
		else
			debug_draw->toggle();
	}

	if (evt.isKeyPressed(rhea::Key::N)) {
		if (m_gui_controller) {
			m_gui_controller->dumpDebugInfo();
		}
	}
	return false;
}

rhea::Widget* Ceto::getMainGuiRoot() const
{
	return doCast<rhea::Widget>(m_unit_tree->find("//local/cetogui/gui"));
}

void Ceto::devAddTestLabel()
{
	rhea::Widget* w = getMainGuiRoot();
	if (!w)
		return;

	rhea::Label* test_label =
		simpleparser::Type::create<rhea::Label>("testlabel_" + std::to_string(m_dev_test_counter));

	std::unordered_map<std::string, Variant> values;
	values["x"] = 60;
	values["y"] = 80 + (m_dev_test_counter - 1) * 20;
	values["w"] = 200;
	values["h"] = 50;

	test_label->setup(this, {}, values);
	w->addChild(test_label);
	test_label->ready();
	test_label->setText("This is a test label " + std::to_string(m_dev_test_counter));
	w->invalidateLayout();

	++m_dev_test_counter;
}

void Ceto::devAddTestWidget()
{
	rhea::Widget* w = getMainGuiRoot();
	if (!w)
		return;

	rhea::Widget* testw = simpleparser::Type::create<rhea::Widget>("testwidget");

	std::unordered_map<std::string, Variant> values;
	values["x"] = 100;
	values["y"] = 100;
	values["w"] = 200;
	values["h"] = 50;
	values["texture"] = "//local/darkred.png";

	testw->setup(this, {}, values);
	w->addChild(testw);
	testw->ready();
	w->invalidateLayout();
}

void Ceto::devAddTestButton()
{
	rhea::Widget* w = getMainGuiRoot();
	if (!w)
		return;

	rhea::Button* testbtn = simpleparser::Type::create<rhea::Button>("testbutton");

	std::unordered_map<std::string, Variant> values;
	values["x"] = 100;
	values["y"] = 100;
	values["w"] = 200;
	values["h"] = 50;
	values["texture"] = "//local/darkred.png";
	values["text"] = "Test Button";

	testbtn->setup(this, {}, values);
	w->addChild(testbtn);
	testbtn->ready();
	w->invalidateLayout();
}

void Ceto::devAddTestDialog()
{
	rhea::Widget* w = getMainGuiRoot();
	if (!w)
		return;

	rhea::Dialog* testdlg = simpleparser::Type::create<rhea::Dialog>("testdialog");

	std::unordered_map<std::string, Variant> values;
	values["x"] = 100;
	values["y"] = 100;
	values["w"] = 200;
	values["h"] = 250;
	values["texture"] = "//local/darkred.png";

	testdlg->setup(this, {}, values);
	w->addChild(testdlg);
	testdlg->ready();
	w->invalidateLayout();
}

void Ceto::devToggleSceneAnimation()
{
	rhea::Item* gus2 = doCast<rhea::Item>(m_unit_tree->find("//local/scene1/gus2"));
	if (!gus2)
		return;

	rhea::Animation* anim = nullptr;
	rhea::Mesh* mesh = doCast<rhea::Mesh>(gus2->getChild("mesh"));
	if (mesh) {
		if (!walk_state) {
			anim = doCast<rhea::Animation>(mesh->getChild("IdleAction"));
			walk_state = 1;
		}
		else {
			anim = doCast<rhea::Animation>(mesh->getChild("RunningAction"));
			walk_state = 0;
		}
	}

	rhea::Animator* animator = doCast<rhea::Animator>(m_unit_tree->find("//local/scene1/Animator"));
	if (animator && anim) {
		animator->PlayAnimation(anim);
	}
}

#endif

bool Ceto::onRealmSelect(rhea::Event* ev, rhea::Unit* unit)
{
	ev->stopPropagation();

	rhea::Unit* target = ev->getTarget();
	DLOG(LOG_DEBUG) << "target selected was '" << (std::string)target->getValue("value") << "'";

	openScene((std::string)target->getValue("value"));
	return false;
}

void Ceto::setRealmSelectDialog()
{
	// show dialog
	rhea::Widget* w = doCast<rhea::Widget>(m_unit_tree->find("//local/cetogui/gui/realms"));
	if (w) {
		m_gui_controller->openDialog((rhea::Dialog*)w);
	}
}

void Ceto::openScene(std::string scene_name)
{
	rhea::Widget* w = doCast<rhea::Widget>(m_unit_tree->find("//local/cetogui/gui/realms"));
	if (w) {
		m_gui_controller->closeDialog((rhea::Dialog*)w);
	}
	m_home_scene = scene_name;

	// open the scene
	setScene();	
}

void Ceto::setup()
{
	addDefaultWindow();
	m_windows[0].views[0].layers.push_back({"HUD",0,0,m_windows[0].width, m_windows[0].height});

	rhea::Application::setup();

	// check for config file

	// add defaults

	// read from userprefs
	m_userpref_path = m_fs->getFs()->getAppDataDir();
	m_userpref_path += string("/ceto.json");

	if (m_fs->getFs()->fileExists(m_userpref_path))
	{
		// file exists		
		std::unique_ptr<orion::FileResourceStream> stream = std::make_unique<orion::FileResourceStream>(m_userpref_path);
		std::unique_ptr<psio::IOBuffer> buf = std::make_unique<psio::IOBuffer>(64 * 1024);
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
	rhea::Unit* engine = m_unit_tree->createUnit("engine", "//core/types/unit", attributes, values);
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
	rhea::Unit* user = m_unit_tree->createUnit("user", "//core/types/unit", attributes, values);
	engine->addChild(user);

	// a color material - uses the shader resource system
	// The poseidon::Shader will be created lazily by EnsurePoseidonShader
	// when a mesh with this material is first rendered.
	rhea::Material* mat = new rhea::Material("color");
	mat->setup(this);
	mat->setValue("shader", "shader_vp");
	m_unit_tree->addChild(mat);
}

void Ceto::setScene(bool mark_preferrences)
{
	if (mark_preferrences) {
		m_userprefs.lastUsed(m_home_scene);

		std::string s = m_userprefs.toJSON();
		std::unique_ptr<orion::FileResourceStream> stream = std::make_unique<orion::FileResourceStream>(m_userpref_path);
		stream->SetMode(psio::Fs::WRITE);
		stream->Open();
		stream->Write(s.c_str(), s.size());
		stream->Close();
	}

	loadResource(m_home_scene, "//meworla/scene", [&](rhea::Unit* scene_unit, ReqError err) {
		rhea::Scene* scene = doCast<rhea::Scene>(scene_unit);
		if (scene && err == ReqError::OK) {

			// set origin info
			rhea::Unit* engine = doCast<rhea::Unit>(m_unit_tree->getChild("engine"));
			if (engine)
				engine->setValue("origin", m_home_scene);

			// hide GUI background
			rhea::GUIController* w = doCast<rhea::GUIController>(m_unit_tree->find("//local/cetogui/gui"));
			if (w) {
				w->setStyle(rhea::StylePropertyId::kBackgroundImage, "none");
				w->UpdateStyle();
			}				

			inScene = true;

			// display scene
			std::string start_scene = scene->getValueString("startscene", "");
			if (start_scene != "") {
				rhea::Scene* go_scene = doCast<rhea::Scene>(m_unit_tree->find(start_scene, scene->getPath(), false));
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
			rhea::Collection* dir = doCast<rhea::Collection>(scene_unit);
			if (dir) {
				// user tried to open directory
				// try world.scene
				m_home_scene += "/world.scene";
				return setScene(false);
			}

			log("Could not open scene");
			rhea::Widget* w = doCast<rhea::Widget>(m_unit_tree->find("//local/cetogui/gui/realms"));
			if (w) {
				m_gui_controller->closeDialog((rhea::Dialog*)w);
			}
			showAlert("Could not open scene");
		}
		});
}

void Ceto::showScene(rhea::Scene* scene)
{
	// initialize physics
	auto* mgr = static_cast<rhea::PhysicsManagerService*>(m_services["physics_mgr"]);
	if (!mgr) {
		LOG(LOG_ERROR) << "No physics manager service found. Physics will be disabled.";		
	}
	else { 
		std::string backend = "none";
		std::string phy = scene->getValueString("physics", "none");
		if (phy == std::string("world") || phy == std::string("bullet")) {
			backend = "world";
		}
		else if (phy == std::string("none")) {
			backend = "none";
		}
		else if (!phy.empty()) {
			// allow future backends by id, e.g. "physx"
			backend = phy;
		}

		// create service by id (manager handles fallback to "none" if unknown/unavailable)
		rhea::PhysicsService* physics = mgr->createPhysicsService(backend);
		if (!physics) {
			// last-resort fallback if the manager doesn't already do it
			physics = mgr->createPhysicsService("none");
		}

		if (physics) {
			physics->onInit();			
			m_physics = physics;
		}		
	}

	// search for a camera		
	/*	
	m_view = scene->findByType<rhea::Camera>();
	if (!m_view) {
		// no camera found - create a default cam
		m_view = static_cast<rhea::Camera*>(m_unit_tree->createUnit("camera", "//meworla/camera"));
		scene->addChild(m_view);

		rhea::client::GraphicsService* gs = static_cast<rhea::client::GraphicsService*>(getService("graphics"));
		rhea::client::GraphicsManager* mgr = gs->getManager<rhea::Camera>();
		//rhea::client::GraphicsCamera::createElement(core, m_view);
		mgr->Ensure(core, m_view);
		Variant pos = scene->getValue("start");
		if (pos != Variant()) {
			math::Vector3 start_pos(pos[0], pos[1], pos[2]);
			m_view->setPosition(start_pos);
		}
		else {
			m_view->setPosition(math::Vector3(0, 1.7f, 0.0f));
		}
	}
	*/

	// search for a controller
	m_focused_controller = m_unit_tree->findByType<rhea::Controller>();
	if (!m_focused_controller) {
		// no controller found - use camera and add default controller for it
		// note: spectator does not work, because we have no controlled element
		m_focused_controller = new rhea::BasicController("camera_controller", "//meworla/controller");
		m_focused_controller->setup(this);
		m_focused_controller->connect(m_view);
		scene->addChild(m_focused_controller);
	}

	// now we are ready
	scene->show();

	rhea::Unit* gus2 = doCast<rhea::Unit>(scene->getChild("gus2"));
	if (gus2)
	{
		rhea::Mesh* mesh = doCast<rhea::Mesh>(gus2->getChild("mesh"));
		if (mesh) {
			rhea::Animation* anim = doCast<rhea::Animation>(mesh->getChild("RunningAction"));
			rhea::Skeleton* skeleton = doCast<rhea::Skeleton>(mesh->getComponent<rhea::Skeleton>());
			if (anim && skeleton) {
				// add the animator to the scene
				rhea::Animator* animator = new rhea::Animator("Animator", "//meworla/animator");
				animator->setSkeleton(skeleton);
				animator->PlayAnimation(anim);
				scene->addChild(animator);
			}
		}
	}
}

void Ceto::showAlert(std::string text)
{
	// set the text
	m_gui->setValue("alert", text);
	 
	// show dialog
	rhea::Widget* w = doCast<rhea::Widget>(m_unit_tree->find("//local/cetogui/gui/alert"));
	if (w) {
		w->show();
	}
}

void Ceto::loadMainGUI()
{
	// load the apps gui			
	std::string app_gui = "//local/cetogui.scene";
	loadResource(app_gui, "//meworla/scene", [&](rhea::Unit* res, ReqError r_err) {
		rhea::Scene* gui = doCast<rhea::Scene>(res);
		if (gui && r_err == ReqError::OK) {
			
			gui->show();
			m_gui = gui;
			m_gui_controller = doCast<rhea::GUIController>(gui->find("gui"));
			log("gui ready");
			
			// find label for debug text
			m_debug_text = doCast<rhea::Label>(m_unit_tree->find("//local/cetogui/gui/widget1/laber"));
			if (!m_debug_text) {
				log("no debug text label found");
			}

			// setup system dialogs

			// quit dialog
			rhea::Dialog* quit_dlg = doCast<rhea::Dialog>(m_unit_tree->find("//local/cetogui/gui/quit"));
			if (quit_dlg) {
				quit_dlg->registerEventHandler("submitted", this, std::bind(&Ceto::onQuit, this, std::placeholders::_1, std::placeholders::_2));
				quit_dlg->registerEventHandler("canceled", this, std::bind(&Ceto::onQuitCancel, this, std::placeholders::_1, std::placeholders::_2));
			}
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

bool Ceto::checkLocalScene(std::string realm_name, std::string path)
{
	return m_fs->checkResourceFromCache(realm_name, path);
}

bool Ceto::onFrameEnd()
{
	static float elapsed_s = 0.0f;

	if (m_debug_text) {
		float elapsed_d_s = static_cast<float>(elapsed_ticks) / 1000000.0f;
		elapsed_s += elapsed_d_s;

		std::string debug = "";
		debug += "debugging: sec=" + std::to_string(elapsed_s) +"; fps = " + std::to_string(floor(fps*100)/100.0f);
		m_focused_controller = doCast<rhea::BasicController>(m_unit_tree->find("//local/scene1/maincamera"));
		if (m_focused_controller) {
			math::Vector3 pos = static_cast<rhea::BasicController*>(m_focused_controller)->getPosition();
			math::Quaternion rot = static_cast<rhea::BasicController*>(m_focused_controller)->getOrientation();
			float angle;
			math::Vector3 axis;
			rot.getAsAngleAxis(angle, axis);
			debug += "\nPosition: " + std::to_string(pos.x) + " / " + std::to_string(pos.y) + " / " + std::to_string(pos.z);
			debug += "\nRotation angle=" + std::to_string(angle * math::M_RADTODEG);
			// (vector is always 0,1,0) 
			//debug += "\nRotation: Angle=" + std::to_string(angle * math::M_RADTODEG) + "\nAxes: " + std::to_string(axis.x) + " / " + std::to_string(axis.y) + " / " + std::to_string(axis.z);
		}
		m_debug_text->setText(debug);
	}

	return m_exiting;
}

void Ceto::onScene(rhea::Scene* scene)
{
	log("scene displayed");
	std::unique_ptr<rhea::Event> ev_r = std::make_unique<rhea::Event>("ready", scene);
	scene->sendEvent(std::string("ready"), ev_r.get());

	m_current_scene = scene;
}

int main(int argc, char* argv[])
{
	Ceto* app = new Ceto();
	app->setAppname("Ceto");
	int argi = 1;
	bool scene_set = false;

	app->init(argc, argv);

	rhea::ConfigFile* config = app->getConfig();
	if (config) {
		// scene set in config file has highest priority
		std::string scene = config->getValueString("scene", "");
		if (scene != "") {
			app->setHomeScene(scene);
			scene_set = true;
		}
	}

	app->setup();

	if (!scene_set) {

		// try world scene in root
		if (app->checkLocalScene("", "world.scene"))
			app->setHomeScene("world.scene");
		else {

			// try ceto world scene
			if (app->checkLocalScene("ceto", "/world.scene"))
				app->setHomeScene("//ceto/world.scene");
			else {

				// no template found - show error
				printf("Could not open scene file. No world.scene file found");
				exit(-1);
			}
		}
	}
	app->loadMainGUI();
		
	if (scene_set) {
		rhea::Widget* w = doCast<rhea::Widget>(app->getUnitTree()->find("//local/cetogui/gui/realms"));
		if (w) {
			w->hide();
		}
		app->setScene();
	}

	app->loop();	

	app->close();

	delete app;

	return 0;
}