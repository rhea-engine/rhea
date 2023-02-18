#ifndef CETO_H
#define CETO_H

#include <string>

#include <Poseidon/Poseidon.h>
#include <Rhea/Fabrics/3D/Camera.h>
#include <Rhea/Fabrics/GUI/Label.h>

using std::string;

class Ceto : public Poseidon::Application
{
protected:
	string m_home_scene;
	Rhea::Camera* m_view = 0;
	Rhea::Label* m_debug_text = nullptr;
public:
	void setup();
	void showScene(Rhea::Scene* scene);
	void setHomeScene(string scene);
	bool onFrameEnd();
};

#endif // !CETO_H
