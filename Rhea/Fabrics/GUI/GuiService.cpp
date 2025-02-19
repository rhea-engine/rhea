#include "GuiService.h"
#include "GUIController.h"

namespace Rhea
{
	void GuiService::onInit()
	{
		return;
	}

	bool GuiService::onFrame(float delta)
	{	
		for (auto controller : m_controllers) {
			controller->UpdateStyle();
		}
		return false;
	}

	bool GuiService::onDisplay(Event* ev, Unit* unit)
	{
		return false;
	}

	void Rhea::GuiService::unregisterController(GUIController* ctrl)
	{
		m_controllers.erase(std::remove(m_controllers.begin(), m_controllers.end(), ctrl));
	}
}
