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
#include <Rhea/Core/WorldService.h>

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
#include <Rhea/GUI/TextInput.h>
#include <RheaAudio/AudioService.h>
#include <Rhea/Animation/AnimationComponent.h>
#include <Rhea/Components/MeshComponent.h>
#include <Rhea/Controller/ControllerService.h>

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

	if (evt.isKeyPressed(rhea::Key::G)) {
		rhea::Item* gus2 = doCast<rhea::Item>(m_current_scene->getChild("gus2"));
		rhea::AnimationComponent* ac = gus2->getComponent<rhea::AnimationComponent>();
		if (ac) {
			if (ac->isPlaying()) {
				ac->pause();
			}
			else {
				ac->resume();
			}
		}
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

	if (evt.isKeyPressed(rhea::Key::B)) {
		// transition to a new scene
		auto* worlds = doCast<rhea::WorldService>(getService("world"));
		std::string scene_path = "//local/world1_scene2.scene";
		if (worlds) {
			if (worlds->getPrimaryScene()->getPathName() == "scene2") {
				scene_path = "//local/world1_scene1.scene";
			}
			const rhea::WorldService::TransitionRequest request = {
				scene_path,
				"entry_point",
				{},
				false
			};
			worlds->transitionToScene(request);
		}
	}
	if (evt.isKeyPressed(rhea::Key::V)) {
		// activate a new scene
		auto* worlds = doCast<rhea::WorldService>(getService("world"));
		std::string scene_path_a = "//local/world1_scene1_a.scene";
		static bool scene1_a_active = false;
		if (worlds) {
			if (scene1_a_active) {
				scene1_a_active = false;
				worlds->deactivateScene(scene_path_a);
			}
			else {
				scene1_a_active = true;
				worlds->activateScene(scene_path_a);
			}
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
	rhea::Item* gus2 = doCast<rhea::Item>(m_current_scene->getChild("gus2"));
	rhea::AnimationComponent* ac = gus2->getComponent<rhea::AnimationComponent>();
	if (!gus2 || !ac)
		return;

	std::string current_state = ac->getCurrentStateName();

	// toggle between idle and running animation
	if (current_state == "idle") {
		ac->play("running");
	}
	else {
		ac->play("idle");
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

bool Ceto::onRealmSearch(rhea::Event* ev, rhea::Unit* unit)
{
	ev->stopPropagation();
	rhea::TextInput* target = doCast<rhea::TextInput>(ev->getTarget());
	if (!target) {
		DLOG(LOG_DEBUG) << "onRealmSearch: target is not a TextInput";
		return false;
	}
	DLOG(LOG_DEBUG) << "search text was '" << (std::string)target->getText() << "'";
	openScene((std::string)target->getText());
	// search for realms
	/*
	rhea::Widget* w = doCast<rhea::Widget>(m_unit_tree->find("//local/cetogui/gui/realms"));
	if (w) {
		rhea::List* list = doCast<rhea::List>(w->find("realm_list"));
		if (list) {
			list->clearItems();
			// add search result items
			for (int i = 0; i < 5; i++) {
				std::string realm_name = "Realm " + std::to_string(i + 1);
				rhea::Unit* item = m_unit_tree->createUnit("realm_item_" + std::to_string(i), "//core/types/unit");
				item->setValue("text", realm_name);
				item->setValue("value", realm_name);
				list->addItem(item);
			}
		}
	}
	*/
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

	// register contoller service
	if (!getService("controller")) {
		log("Ceto: Registering controller service");
		rhea::ControllerService* srv = new rhea::ControllerService("controllerservice");
		srv->setup(this);
		registerService("controller", srv);
	}

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

	auto* worlds = doCast<rhea::WorldService>(getService("world"));
	worlds->addWorldPrepareHandler([this, worlds](rhea::WorldService::WorldPrepareContext* context) -> rhea::WorldService::PrepareError
		{
			if (context->world)
				m_focused_controller = context->world->findByType<rhea::Controller>();
			else if (context->standaloneScene)
				m_focused_controller = context->standaloneScene->findByType<rhea::Controller>();
			else
				m_focused_controller = nullptr;
			return rhea::WorldService::PrepareError::None;
		});
	worlds->addSceneTransitionHandler([this, worlds](rhea::WorldService::SceneTransitionContext* context) -> rhea::WorldService::TransitionError
		{
			return rhea::WorldService::TransitionError::None;
		});
	auto* ctrl_srv = getService<rhea::ControllerService>();
	ctrl_srv->addPlacementHandler([this, ctrl_srv](rhea::ControllerService::PlacementContext& context) -> rhea::ControllerService::PlacementResult
		{
			if (!context.source_character || !context.target_character)
				return rhea::ControllerService::PlacementResult::Unhandled;
#ifdef CETO_TEST_PLACEMENT
			auto position = context.source_character->getTransform()->getWorldPosition();
			context.target_character->getTransform()->setWorldPosition(position);
			return rhea::ControllerService::PlacementResult::Placed;
#else
			return rhea::ControllerService::PlacementResult::Unhandled;
#endif;
		});
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

	auto* worlds = doCast<rhea::WorldService>(getService("world"));

	worlds->openWorld(m_home_scene,	[this, worlds](rhea::WorldService::OpenError error)
		{
			if (error != rhea::WorldService::OpenError::None) {
				log("Ceto: Could not open scene");
				rhea::Widget* w = doCast<rhea::Widget>(m_unit_tree->find("//local/cetogui/gui/realms"));
				if (w) {
					m_gui_controller->closeDialog((rhea::Dialog*)w);
				}
				showAlert("Could not open world");
				return;
			}
			
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
		});
	
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
	loadResource(app_gui, "//meworla/scene", [&](rhea::Component* res, ReqError r_err) {
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
			gui->registerEventHandler("search", this, std::bind(&Ceto::onRealmSearch, this, std::placeholders::_1, std::placeholders::_2));
		}
		else {
			log("loading of gui failed");
		}
		});
}

void Ceto::setHomeScene(std::string scene)
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
		if (m_focused_controller) {
			rhea::Item* target = m_focused_controller->getTarget();
			math::Vector3 pos = target->getTransform()->getWorldPosition().getTranslation();
			math::Quaternion rot = target->getTransform()->getWorldPosition().getRotation();
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
	LOG(LOG_DEBUG) << "Ceto::onScene: scene '" << scene->getPathName() << "' is ready";
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