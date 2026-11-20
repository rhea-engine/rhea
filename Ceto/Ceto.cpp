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
#include <Rhea/GUI/Label.h>
#include <RheaAudio/AudioService.h>
#include <Rhea/Animation/AnimationComponent.h>
#include <Rhea/Components/MeshComponent.h>
#include <Rhea/Controller/ControllerService.h>
#include <Rhea/Core/SceneResource.h>

#include "Gui/OpenBrowser.h"
#include "Gui/AlertDialog.h"
#include "Gui/CetoUI.h"
#include "AssetViewer.h"
#include "BrowserController.h"

Ceto::~Ceto()
{
}

bool Ceto::onQuit(rhea::Event* ev, rhea::Unit* unit)
{
	ev->stopPropagation();

	// user confirmed quit
	m_exiting = true;

	return true;
}

bool Ceto::onInput(rhea::EventInput* e, rhea::Unit* unit)
{
	rhea::InputEvent evt = e->getEvent();

	if (handleAppShortcuts(evt)) {
		e->stopPropagation();
		e->consumeTextForKeyPress(true);
		return false;
	}

	return false;
}

bool Ceto::handleAppShortcuts(const rhea::InputEvent& evt)
{
	if (evt.isKeyPressed(rhea::Key::Escape)) {		
		m_ceto_ui->showQuitDialog();
		return true;
	}

	if (evt.isKeyPressed(rhea::Key::O)) {
		m_ceto_ui->showOpenDialog();
		return true;
	}
	
	return false;
}

void Ceto::onOpened(const std::string& address, bool mark_preferences)
{
	setOrigin(address);

	if (mark_preferences) {
		m_userprefs.lastUsed(address);

		std::string s = m_userprefs.toJSON();
		std::unique_ptr<orion::FileResourceStream> stream = std::make_unique<orion::FileResourceStream>(m_userpref_path);
		stream->SetMode(psio::Fs::WRITE);
		stream->Open();
		stream->Write(s.c_str(), s.size());
		stream->Close();

		m_ceto_ui->setRecentLocations(m_userprefs.lastUsed());
	}
}

void Ceto::setup()
{
	addDefaultWindow();
	m_windows[0].views[0].layers.push_back({"HUD",0,0,m_windows[0].width, m_windows[0].height});

	OpenBrowser::_register_OpenBrowser();
	AlertDialog::_register_AlertDialog();
	CetoUI::_register_CetoUI();

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

	rhea::Unit* engine = doCast<rhea::Unit>(m_unit_tree->find("engine"));
	if (!engine) {
		engine = m_unit_tree->createUnit("engine", "//core/types/unit", attributes, values);
		m_unit_tree->addChild(engine);
	}

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
	mat->setup(this, {}, {});
	mat->setValue("shader", "shader_vp");
	m_unit_tree->addChild(mat);

	auto* worlds = doCast<rhea::WorldService>(getService("world"));

	worlds->addWorldActivateHandler([this, worlds](rhea::WorldService::WorldPrepareContext* context) -> rhea::Error
		{
			if (context->world)
				m_focused_controller = context->world->findByType<rhea::Controller>();
			else if (context->standalone_scene)
				m_focused_controller = context->standalone_scene->findByType<rhea::Controller>();
			else
				m_focused_controller = nullptr;
			return rhea::Error();
		});
	worlds->addSceneTransitionPrepareHandler([this, worlds](rhea::WorldService::SceneTransitionContext* context, rhea::WorldService::PrepareCompletion completion) -> void
		{
			completion(rhea::Error());
		});
	worlds->addSceneTransitionActivateHandler([this](rhea::WorldService::SceneTransitionContext* context) {
			if (!context->target_scene)
				return;
			m_current_scene = context->target_scene;

			return;
			context->target_scene->traverse([](std::string& name, rhea::Component* component) -> bool {
				LOG(LOG_DEBUG) << "Scene component: " << component->getParent()->getPath() << "/" << name << " (" << component->getType()->getName() << ")";
				return true;
				});
		});
	worlds->setOpenResourceAdapter([this](const rhea::WorldService::OpenResourceContext& context, rhea::WorldService::AdaptResourceCompletion completion) {
		if (m_asset_viewer == nullptr)
			m_asset_viewer = std::make_unique<AssetViewer>();
		m_asset_viewer->createAssetViewerScene(this, context, completion);
		});
}

void Ceto::openScene(std::string scene_name, bool mark_preferences)
{
	m_browser_controller->openScene(scene_name, mark_preferences);
}

void Ceto::loadMainGUI(CompletionHandler handler)
{
	// load the apps gui			
	std::string app_gui = "//ceto/cetogui.scene";
	loadResource(app_gui, "//meworla/scene", [handler, this](rhea::Component* res, rhea::ErrorReport r_err) {
		rhea::SceneResource* scene_res = doCast<rhea::SceneResource>(res);
		if (scene_res && !r_err.isError()) {
			rhea::Unit* parent = doCast<rhea::Unit>(m_unit_tree->find(scene_res->getBaseId()));
			scene_res->spawn(parent, [handler, this](rhea::ErrorReport r_err, rhea::Unit* res) {
				rhea::Scene* gui = doCast<rhea::Scene>(res);
				if (gui && !r_err.isError()) {

					// show the gui

					gui->show();
					m_gui = gui;
					m_gui_controller = doCast<rhea::GUIController>(gui->find("gui"));

					if (!m_browser_controller) {
						m_browser_controller = std::make_unique<BrowserController>(this);
						m_browser_controller->addOpenedHandler(std::bind(&Ceto::onOpened, this, std::placeholders::_1, std::placeholders::_2));
					}

					m_ceto_ui = doCast<CetoUI>(gui->find("ceto_ui"));
					m_ceto_ui->initializeBrowser(m_browser_controller.get());

					log("gui ready");

					// setup system dialogs
					m_ceto_ui->registerEventHandler("quit_application", this, std::bind(&Ceto::onQuit, this, std::placeholders::_1, std::placeholders::_2));

					m_ceto_ui->setOpenHandler([this](const std::string& address) {
						m_browser_controller->openScene(address);
						});

					// open browser setup


					BrowserData browser_data;

					browser_data.recent_locations = m_userprefs.lastUsed();

					browser_data.places = {
						{ "Most Recent", "//recent/", BrowserPlaceKind::Internal },
						{ "User",        "//user/",   BrowserPlaceKind::Location },
					};
					if (m_fs->hasLocalDir()) {
						browser_data.places.push_back({ "Local", "//local/", BrowserPlaceKind::Location });
					}
					browser_data.places.push_back({ "Computer", "file:///", BrowserPlaceKind::Location });
					std::string samples_path = m_fs->getFs()->getInstallDataDir();
					if (samples_path.length()) {
						samples_path += "/samples";
						if (psio::Fs::fileExists(samples_path)) {
							m_fs->addLocalRealm("samples", samples_path);
							browser_data.places.push_back({ "Samples", "//samples/", BrowserPlaceKind::Location });
						}
					}
					std::unordered_map<std::string, bool> realms;
					realms["ceto"] = true;
					realms["recent"] = true;
					realms["user"] = true;
					realms["local"] = true;
					realms["file"] = true;
					realms["samples"] = true;
					auto get_realm_name = [this](const std::string& address) -> std::string {
						if (address.compare(0, 2, "//") != 0)
							return {};
						const auto realm_end = address.find('/', 2);
						return address.substr(2, realm_end == std::string::npos ? std::string::npos : realm_end - 2);
						};
					for (auto it = m_userprefs.lastUsed().begin(); it != m_userprefs.lastUsed().end(); it++) {
						std::string realm_name = get_realm_name(*it);
						if (realm_name.empty())
							continue;
						if (realms.find(realm_name) == realms.end()) {
							realms[realm_name] = true;
							browser_data.places.push_back({ realm_name, "//" + realm_name + "/", BrowserPlaceKind::Location });
						}
					}

					m_ceto_ui->setBrowserData(browser_data);
					m_ceto_ui->navigateOpenDialogTo("//recent/");

					handler(rhea::Error());
				}
				else {
					log("instantiating of gui failed");
					handler(r_err);
				}
				});
		}
		else {
			log("loading of gui failed");
			handler(r_err);
		}
		});
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
			if (target) {
				math::Vector3 pos = target->getTransform()->getWorldPosition().getTranslation();
				math::Quaternion rot = target->getTransform()->getWorldPosition().getRotation();
				float angle;
				math::Vector3 axis;
				rot.getAsAngleAxis(angle, axis);
				debug += "\nPosition: " + std::to_string(pos.x) + " / " + std::to_string(pos.y) + " / " + std::to_string(pos.z);
				debug += "\nRotation angle=" + std::to_string(angle * math::radToDeg);
			}
			// (vector is always 0,1,0) 
			//debug += "\nRotation: Angle=" + std::to_string(angle * math::radToDeg) + "\nAxes: " + std::to_string(axis.x) + " / " + std::to_string(axis.y) + " / " + std::to_string(axis.z);
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
}

void Ceto::openBrowser()
{
	m_ceto_ui->showOpenDialog();
}

void Ceto::setOrigin(std::string origin)
{
	rhea::Unit* engine = doCast<rhea::Unit>(m_unit_tree->getChild("engine"));
	if (engine)
		engine->setValue("origin", origin);
}

int main(int argc, char* argv[])
{
	Ceto* app = new Ceto();
	app->setAppname("Ceto");
	int argi = 1;
	std::string set_scene;

	app->init(argc, argv);

	rhea::ConfigFile* config = app->getConfig();
	if (config) {
		// scene set in config file or argument has highest priority
		std::string scene = config->getValueString("scene", "");
		if (scene != "") {
			set_scene = scene;
		}
	}

	app->setup();

	if (set_scene.empty()) {
		// try world scene in root
		if (app->checkLocalScene("", "world.scene"))
			set_scene = "world.scene";
	}

	app->loadMainGUI([set_scene, app](rhea::ErrorReport report){
		if (report.isError()) {
			for (auto& err : report.errors) {
				LOG(LOG_ERROR) << "Error loading main GUI: " << err.getMessage() << " (" << err.getResourceId() << ")";
			}
			exit(-1);
		}
		if (!set_scene.empty()) {
			// open the scene specified in config file or command line argument
			app->openScene(set_scene, false);
		}
		else {
			// open the open browser
			app->openBrowser();
		}
	});

	app->loop();	

	app->close();

	delete app;

	return 0;
}
