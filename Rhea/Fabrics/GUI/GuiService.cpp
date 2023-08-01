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
				static_cast<Item*>(unit)->onFrame(delta);
				return true;
				});
		}
		
		return false;
	}

	bool GuiService::onDisplay(Event* ev, Unit* unit)
	{
		return false;
	}

	// factory function
	Unit* GuiService::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new GuiService(name, type);
		return ret;
	}

	void Rhea::GuiService::unregisterController(GUIController* ctrl)
	{
		remove(m_controllers.begin(), m_controllers.end(), ctrl);
	}
}
