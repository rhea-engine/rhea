/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef CETO_H
#define CETO_H

#include <string>

#include <Rhea/Application.h>
#include <Rhea/Core/Camera.h>
#include <Rhea/GUI/Label.h>
#include <Rhea/GUI/GUIController.h>
#include <Rhea/EventInput.h>
#include "UserPrefs.h"

using std::string;

class OpenBrowser;
class AlertDialog;
class AssetViewer;
class CetoUI;
class BrowserController;
class ViewerSession;

class Ceto : public rhea::Application
{
public:
	using CompletionHandler = std::function<void(rhea::ErrorReport)>;

	Ceto() :
		m_gui(nullptr),
		m_gui_controller(nullptr),
		m_view(nullptr),
		m_debug_text(nullptr),
		m_exiting(false),
		m_current_time(0.0f)
	{}
	~Ceto();
	void setup();
	void openScene(std::string scene_name, bool mark_preferences = true);
	bool checkLocalScene(std::string realm_name, std::string path);
	void loadMainGUI(CompletionHandler handler);
	
	bool onFrameEnd();
	void onScene(rhea::Scene* scene) override;
	void openBrowser();

	void setOrigin(std::string origin);

protected:
	rhea::Camera* m_view;
	rhea::Label* m_debug_text;
	bool m_exiting;

	// event functions
	bool onQuit(rhea::Event* ev, rhea::Unit* unit);
	bool onInput(rhea::EventInput* e, rhea::Unit* unit);
	void showAlert(std::string text);

	
	// shortcut helpers
	bool handleAppShortcuts(const rhea::InputEvent& evt);
	bool beginOrbitDrag(rhea::Event* event, rhea::Unit*);
	bool updateOrbitDrag(rhea::Event* event, rhea::Unit*);

	bool m_orbit_dragging = false;
	int m_orbit_mouse_x = 0;
	int m_orbit_mouse_y = 0;

	// user preferences
	UserPrefs m_userprefs;
	std::string m_userpref_path;

	// cache of ceto gui scene
	rhea::Scene* m_gui;
	// cache of gui controller
	rhea::GUIController* m_gui_controller;


	CetoUI* m_ceto_ui = nullptr;
	
	std::unique_ptr<AssetViewer> m_asset_viewer;
	std::unique_ptr<ViewerSession> m_viewer_session;

	float m_current_time;

	std::unique_ptr<BrowserController> m_browser_controller;
	void onOpened(const std::string&, bool);
};

#endif // !CETO_H
