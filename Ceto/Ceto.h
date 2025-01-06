#ifndef CETO_H
#define CETO_H

#include <string>

#include <Poseidon/Poseidon.h>
#include <Rhea/Fabrics/3D/Camera.h>
#include <Rhea/Fabrics/GUI/Label.h>
#include "UserPrefs.h"

using std::string;

class Ceto : public Poseidon::Application
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
	void showScene(Rhea::Scene* scene);
	bool checkLocalScene(std::string realm_name, std::string path);
	void loadMainGUI();
	void setHomeScene(string scene);
	bool onFrameEnd();

protected:
	string m_home_scene;
	Rhea::Camera* m_view;
	Rhea::Label* m_debug_text;
	bool m_exiting;

	// event functions
	bool onQuit(Rhea::Event* ev, Rhea::Unit* unit);
	bool onInput(Rhea::EventInput* e, Rhea::Unit* unit);
	bool onRealmSelect(Rhea::Event* ev, Rhea::Unit* unit);
	void showAlert(std::string text);
	void setRealmSelectDialog();
	void openScene(std::string scene_name);
	
	// user preferences
	UserPrefs m_userprefs;
	std::string m_userpref_path;

	// cache of ceto gui scene
	Rhea::Scene* m_gui;

	float m_current_time;
	int walk_state;
};

#endif // !CETO_H
