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
#include "AssetViewer.h"

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
	m_gui_controller->closeDialog(m_quit_dialog);
	// show realms dialog again
	if (!m_in_scene) {
		// if we are not in a scene, show the realms dialog again
		// if we are in a scene, we just stay there and ignore the quit request
		openRealmDialog();
		return true;
	}
	return false;
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

	if (!m_quit_dialog)
		return;

	m_gui_controller->openDialog(m_quit_dialog);
}

void Ceto::openRealmDialog()
{
	if (!m_gui_controller)
		return;

	if (!m_realm_dialog)
		return;

	m_gui_controller->openDialog(m_realm_dialog);
}

void Ceto::openDetailsDialog(rhea::ErrorReport report)
{
	if (!m_gui_controller || !m_details_dialog)
		return;

	std::string title = "Could not open world";
	rhea::Label* title_label = doCast<rhea::Label>(m_details_dialog->findByName("title"));
	if (title_label) {
		title_label->setText(title);
	}

	rhea::Label* message_label = doCast<rhea::Label>(m_details_dialog->findByName("message"));
	if (message_label) {
		message_label->setText("The world could not be opened due to the following errors:");
	}

	std::string message;
	for (auto& err : report.errors) {
		message += "\n\n" + err.getMessage() + "\n" + err.getResourceId();
	}

	rhea::Label* label = doCast<rhea::Label>(m_details_dialog->findByName("details_text"));
	if (label) {
		label->setText(message);
	}

	m_gui_controller->openDialog(m_details_dialog);
}


void Ceto::setup()
{
	addDefaultWindow();
	m_windows[0].views[0].layers.push_back({"HUD",0,0,m_windows[0].width, m_windows[0].height});

	OpenBrowser::_register_OpenBrowser();
	AlertDialog::_register_AlertDialog();

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
	auto* worlds = doCast<rhea::WorldService>(getService("world"));

	worlds->openWorld(scene_name, [this, scene_name, mark_preferences, worlds](rhea::ErrorReport error) {

		if (error.isError()) {
			LOG(LOG_ERROR) << "Ceto: Could not open scene: " << scene_name;

			if (!m_alert_dialog || error.isCanceled())
				return;

			AlertDialog::Content content;
			content.title = "Could not open world";
			if (error.errors.size() == 1) {
				rhea::Error err = error.errors[0];
				content.message = err.getMessage() + "\n" + err.getResourceId();
				content.actions = {
					{ "ok", "OK", true }
				};
			} else {
				content.message = std::to_string(error.errors.size()) + " problems occurred while loading this world.";
				content.actions = {
					{ "details", "Details...", false },
					{ "ok", "OK", true }
				};
			}
			content.default_action = "ok";
			content.cancel_action = "ok";

			if (m_alert_dialog->configure(std::move(content), [this, error](std::string action_id) {
					if (action_id == "details")
						openDetailsDialog(error);
				}))
			{
				m_gui_controller->openDialog(m_alert_dialog);
			}
			return;


			std::string message = "Could not open world.";

			for (auto& err : error.errors) {
				message += "\n\nError: " + err.getMessage() + "\n" + err.getResourceId();
			}
			showAlert(message);
			return;
		}

		if (m_realm_dialog) {
			m_gui_controller->closeDialog(m_realm_dialog);
		}

		// set origin info
		rhea::Unit* engine = doCast<rhea::Unit>(m_unit_tree->getChild("engine"));
		if (engine)
			engine->setValue("origin", scene_name);

		if (mark_preferences) {
			m_userprefs.lastUsed(scene_name);

			std::string s = m_userprefs.toJSON();
			std::unique_ptr<orion::FileResourceStream> stream = std::make_unique<orion::FileResourceStream>(m_userpref_path);
			stream->SetMode(psio::Fs::WRITE);
			stream->Open();
			stream->Write(s.c_str(), s.size());
			stream->Close();
		}

		if (m_realm_dialog)
			m_realm_dialog->setRecentWorlds(m_userprefs.lastUsed());

		// hide GUI background
		rhea::GUIController* w = doCast<rhea::GUIController>(m_unit_tree->find("//ceto/cetogui/gui"));
		if (w) {
			w->setStyle(rhea::StylePropertyId::kBackgroundImage, "none");
			w->UpdateStyle();
		}

		m_in_scene = true;
		});
}

void Ceto::showAlert(std::string text)
{
	if (!m_gui_controller)
		return;

	if (!m_alert_dialog) {
		rhea::Widget* w = doCast<rhea::Widget>(m_unit_tree->findByName("alert"));
		if (w) {
			m_alert_dialog = doCast<AlertDialog>(w);
		}

		if (!m_alert_dialog)
			return;

		m_gui->registerEventHandler("close_alert", this, std::bind(&Ceto::onAlertClose, this, std::placeholders::_1, std::placeholders::_2));
	}

	rhea::Label* label = doCast<rhea::Label>(m_alert_dialog->findByName("info"));
	if (!label)
		return;

	// set the text
	label->setText(text);

	// show dialog
	m_gui_controller->openDialog(m_alert_dialog);
}

bool Ceto::onAlertClose(rhea::Event* ev, rhea::Unit* unit)
{
	if (!m_alert_dialog)
		return false;

	m_gui_controller->closeDialog(m_alert_dialog);

	return false;
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
					log("gui ready");

					// setup system dialogs

					// quit dialog
					m_quit_dialog = doCast<rhea::Dialog>(m_unit_tree->find("//ceto/cetogui/gui/quit"));
					if (m_quit_dialog) {
						m_quit_dialog->registerEventHandler("submitted", this, std::bind(&Ceto::onQuit, this, std::placeholders::_1, std::placeholders::_2));
						m_quit_dialog->registerEventHandler("canceled", this, std::bind(&Ceto::onQuitCancel, this, std::placeholders::_1, std::placeholders::_2));
					}
					gui->registerEventHandler("quit", this, std::bind(&Ceto::onQuit, this, std::placeholders::_1, std::placeholders::_2));

					// realm selection dialog (open browser)

					auto* open_browser_res = doCast<rhea::SceneResource>(m_unit_tree->find("//ceto/openbrowser.scene"));
					if (open_browser_res && m_gui_controller) {
						open_browser_res->spawn(m_gui_controller, "openbrowser", [this, handler](rhea::ErrorReport error, rhea::Unit* instance) {
							m_realm_dialog = doCast<OpenBrowser>(instance);
							if (error.isError() || !m_realm_dialog) {
								log("Could not create open browser");
								handler(error);
								return;
							}
							m_realm_dialog->setOpenHandler([this](const std::string& address) {
								if (address.substr(0, 7) == "//file/") {
									// transofrm legacy address to file:
									openScene("file:///" + address.substr(7));
								}
								else {
									openScene(address);
								}
								});
							m_realm_dialog->setRecentWorlds(m_userprefs.lastUsed());
							std::vector<OpenBrowser::BrowserPlace> places;
							places = {
								{ "Most Recent", "//recent/", OpenBrowser::BrowserPlaceKind::Internal },
								{ "User",        "//user/",   OpenBrowser::BrowserPlaceKind::Location },
							};
							if (m_fs->hasLocalDir()) {
								places.push_back({ "Local", "//local/", OpenBrowser::BrowserPlaceKind::Location });
							}
							places.push_back({ "Computer", "file:///", OpenBrowser::BrowserPlaceKind::Location });
							std::string samples_path = m_fs->getFs()->getInstallDataDir();
							if (samples_path.length()) {
								samples_path += "/samples";
								if (psio::Fs::fileExists(samples_path)) {
									m_fs->addLocalRealm("samples", samples_path);
									places.push_back({ "Samples", "//samples/", OpenBrowser::BrowserPlaceKind::Location });
								}
							}
							std::unordered_map<std::string, bool> realms;
							realms["ceto"] = true;
							realms["recent"] = true;
							realms["user"] = true;
							realms["local"] = true;
							realms["file"] = true;
							realms["samples"] = true;
							for (auto it = m_userprefs.lastUsed().begin(); it != m_userprefs.lastUsed().end(); it++) {
								std::string realm_name = m_realm_dialog->getRealmName(*it);
								if (realm_name.empty())
									continue;
								if (realms.find(realm_name) == realms.end()) {
									realms[realm_name] = true;
									places.push_back({ realm_name, "//" + realm_name + "/", OpenBrowser::BrowserPlaceKind::Location });
								}
							}
							m_realm_dialog->setPlaces(places);
							m_realm_dialog->navigateTo("//recent/");
							});
					}

					m_alert_dialog = doCast<AlertDialog>(m_unit_tree->find("//ceto/cetogui/gui/alert"));
					if (!m_alert_dialog) {
						log("no alert dialog found");
					}

					m_details_dialog = doCast<rhea::Dialog>(m_unit_tree->find("//ceto/cetogui/gui/details"));
					if (!m_details_dialog) {
						log("no details dialog found");
					}
					else {
						m_details_dialog->registerEventHandler("submitted", this, [this](rhea::Event* ev, rhea::Unit* unit) {
							m_gui_controller->closeDialog(m_details_dialog);
							return false;
							});
						m_details_dialog->registerEventHandler("canceled", this, [this](rhea::Event* ev, rhea::Unit* unit) {
							m_gui_controller->closeDialog(m_details_dialog);
							return false;
							});
					}

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

void Ceto::hideOpenBrowserDialog()
{
	if (m_realm_dialog) {
		m_gui_controller->closeDialog(m_realm_dialog);
	}
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
			app->openScene(set_scene);
		}
		else {
			// open the open browser
			app->openRealmDialog();
		}
	});

	app->loop();	

	app->close();

	delete app;

	return 0;
}
