#include <exception>

#include "Hippios/Engine.h"
#include "Poseidon/Core.h"
#include "Orion/Orion.h"

#include "Application.h"

namespace Rhea
{
	void Application::init()
	{
		core = new Core();
		m_fs = new Orion::Orion();

		// setup the apps core objects
		setup();
	}

	void Application::setup()
	{
		core->init();
	}

	void Application::loadScene(const std::string filename)
	{
		Orion::Resource* scene_resource;

		try {
			scene_resource = m_fs->getResourceByUrl(filename);
		}
		catch (std::exception e) {
			// file could not be opened
		}

	}

	int Application::loop()
	{
		// start the loop
		int ret = core->loop();

		return ret;
	}

	void Application::close()
	{

	}
}