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
	void setup();
	void openScene(std::string scene_name, bool mark_preferences = true);
	bool checkLocalScene(std::string realm_name, std::string path);
	void loadMainGUI(CompletionHandler handler);
	
	bool onFrameEnd();
	void onScene(rhea::Scene* scene) override;
	void openBrowser();

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

	// user preferences
	UserPrefs m_userprefs;
	std::string m_userpref_path;

	// cache of ceto gui scene
	rhea::Scene* m_gui;
	rhea::Scene* m_current_scene = nullptr;
	// cache of gui controller
	rhea::GUIController* m_gui_controller;


	CetoUI* m_ceto_ui = nullptr;
	
	std::unique_ptr<AssetViewer> m_asset_viewer = nullptr;

	float m_current_time;

	bool m_in_scene = false;
};

#endif // !CETO_H
