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

class Ceto : public rhea::Application
{
public:
	Ceto() :
		m_gui(nullptr),
		m_gui_controller(nullptr),
		m_view(nullptr),
		m_debug_text(nullptr),
		m_exiting(false),
		m_current_time(0.0f),
		walk_state(0)
#if defined(CETO_DEV_TOOLS)
		, m_dev_test_counter(1)
#endif
	{}
	void setup();
	void setScene(bool mark_preferrences = true);
	void showScene(rhea::Scene* scene);
	bool checkLocalScene(std::string realm_name, std::string path);
	void loadMainGUI();
	void setHomeScene(string scene);
	bool onFrameEnd();
	void onScene(rhea::Scene* scene) override;

protected:
	string m_home_scene;
	rhea::Camera* m_view;
	rhea::Label* m_debug_text;
	bool m_exiting;

	// event functions
	bool onQuit(rhea::Event* ev, rhea::Unit* unit);
	bool onQuitCancel(rhea::Event* ev, rhea::Unit* unit);
	bool onInput(rhea::EventInput* e, rhea::Unit* unit);
	bool onRealmSelect(rhea::Event* ev, rhea::Unit* unit);
	void showAlert(std::string text);
	void setRealmSelectDialog();
	void openScene(std::string scene_name);
	
	// shortcut helpers
	bool handleAppShortcuts(const rhea::InputEvent& evt);
	void openQuitDialog();
	void openRealmDialog();

#if defined(CETO_DEV_TOOLS)
	bool handleDevShortcuts(const rhea::InputEvent& evt);

	rhea::Widget* getMainGuiRoot() const;
	void devToggleSceneAnimation();
	void devAddTestLabel();
	void devAddTestWidget();
	void devAddTestButton();
	void devAddTestDialog();

	int m_dev_test_counter;
#endif

	// user preferences
	UserPrefs m_userprefs;
	std::string m_userpref_path;

	// cache of ceto gui scene
	rhea::Scene* m_gui;
	// cache of gui controller
	rhea::GUIController* m_gui_controller;

	float m_current_time;
	int walk_state;

	bool inScene = false;
};

#endif // !CETO_H
