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
		for (auto it = m_controllers.begin(); it < m_controllers.end(); it++)
		{
			(*it)->traverse([delta](std::string& name, Unit* unit)->bool {
				Item* item = doCast<Item>(unit);
				if (item) item->onFrame(delta);
				return true;
				});
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
