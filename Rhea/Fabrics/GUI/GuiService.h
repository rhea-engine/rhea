#ifndef RHEA_GUI_SERVICE
#define RHEA_GUI_SERVICE

#include "../../Service.h"

namespace Rhea
{
	class GUIController;

	class GuiService : public Service
	{
		MECLASS(GuiService, Service)
	public:
		GuiService::GuiService(std::string name, std::string type_name) :
			Service(name, type_name)
		{}

		// Service events

		void onInit();

		bool onFrame(float delta);

		bool onDisplay(Event* ev, Unit* unit);

		void registerController(GUIController* ctrl) { m_controllers.push_back(ctrl); }
		void unregisterController(GUIController* ctrl); 

	protected:
		std::vector<GUIController*> m_controllers;

	};
}
#endif // !RHEA_GUI_SERVICE
