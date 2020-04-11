#include "Ceto.h"

void Ceto::setup()
{
	Rhea::Application::setup();

	// check for config file

	// load home scene if any
	if (!m_home_scene.length())
	{
		m_home_scene = "//Ceto/Init/init.scene";		
	}
	loadScene(m_home_scene);
}

void Ceto::setHomeScene(string scene)
{
	m_home_scene = scene;
}

int main(int argc, char* argv[])
{
	Ceto* app = new Ceto();
	app->setAppname("Ceto");

	if (argc > 1)
		app->setHomeScene(argv[1]);

	app->init();

	app->loop();	

	app->close();

	return 0;
}