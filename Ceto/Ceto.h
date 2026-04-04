#ifndef CETO_H
#define CETO_H

#include <string>

#include <Poseidon/Poseidon.h>
#include <Rhea/Core/Camera.h>
#include <Rhea/GUI/Label.h>
#include "UserPrefs.h"

using std::string;

class Ceto : public poseidon::Application
{
public:
	Ceto() :
		m_gui(nullptr),
		m_view(nullptr),
		m_debug_text(nullptr),
		m_exiting(false),
		m_current_time(0.0f),
		walk_state(0)
	{}
	void setup();
	void setScene(bool mark_preferrences = true);
	void showScene(rhea::Scene* scene);
	bool checkLocalScene(std::string realm_name, std::string path);
	void loadMainGUI();
	void setHomeScene(string scene);
	bool onFrameEnd();

protected:
	string m_home_scene;
	rhea::Camera* m_view;
	rhea::Label* m_debug_text;
	bool m_exiting;

	// event functions
	bool onQuit(rhea::Event* ev, rhea::Unit* unit);
	bool onInput(rhea::EventInput* e, rhea::Unit* unit);
	bool onRealmSelect(rhea::Event* ev, rhea::Unit* unit);
	void showAlert(std::string text);
	void setRealmSelectDialog();
	void openScene(std::string scene_name);
	
	// user preferences
	UserPrefs m_userprefs;
	std::string m_userpref_path;

	// cache of ceto gui scene
	rhea::Scene* m_gui;

	float m_current_time;
	int walk_state;
};

#endif // !CETO_H
