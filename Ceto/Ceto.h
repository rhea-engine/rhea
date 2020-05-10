#ifndef CETO_H
#define CETO_H

#include <string>

#include "Rhea/Application.h"

using std::string;

class Ceto : public Rhea::Application
{
protected:
	string m_home_scene;
	Rhea::SceneView* m_view = 0;
public:
	void setup();
	void setHomeScene(string scene);
};

#endif // !CETO_H
