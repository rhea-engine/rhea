#include <Rhea/GUI/GuiService.h>
#include <Rhea/GUI/GUIController.h>

namespace rhea
{
	void GuiService::onInit()
	{
		return;
	}

	bool GuiService::onFrame(float delta)
	{	
		for (auto controller : m_controllers) {
			controller->onFrame();
		}
		return false;
	}

	bool GuiService::onDisplay(Event* ev, Unit* unit)
	{
		return false;
	}

	void rhea::GuiService::unregisterController(GUIController* ctrl)
	{
		m_controllers.erase(std::remove(m_controllers.begin(), m_controllers.end(), ctrl));
	}
}
