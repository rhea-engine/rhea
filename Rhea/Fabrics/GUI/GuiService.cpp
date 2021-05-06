#include "GuiService.h"

namespace Rhea
{
	void GuiService::onInit()
	{
		return;
	}

	bool GuiService::onFrame()
	{	
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
}
