#ifndef CETO_H
#define CETO_H

#include <string>

#include "Rhea/Application.h"
#include "Rhea/Camera.h"
#include <Rhea/Fabrics/GUI/Label.h>

using std::string;

class Ceto : public Rhea::Application
{
protected:
	string m_home_scene;
	Rhea::Camera* m_view = 0;
	Rhea::Label* m_debug_text = nullptr;
public:
	void setup();
	void setHomeScene(string scene);
	bool onFrameEnd();
};

#endif // !CETO_H
