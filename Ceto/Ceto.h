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
protected:
	string m_home_scene;
	Rhea::Camera* m_view = 0;
	Rhea::Label* m_debug_text = nullptr;
	bool onQuitDialog(Rhea::Event* ev, Rhea::Unit* unit);
	bool onInput(Rhea::EventInput* e, Rhea::Unit* unit);
	bool onRealmSelect(Rhea::Event* ev, Rhea::Unit* unit);
	void setRealmSelectDialog();	
	UserPrefs m_userprefs;
	std::string m_userpref_path;
public:
	void setup();
	void setScene();
	void showScene(Rhea::Scene* scene);
	void loadMainGUI();
	void setHomeScene(string scene);
	bool onFrameEnd();
};

#endif // !CETO_H
