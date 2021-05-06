#ifndef RHEA_GUI_SERVICE
#define RHEA_GUI_SERVICE

#include "../../Service.h"

namespace Rhea
{
	class GuiService : public Service
	{
	public:
		GuiService::GuiService(std::string name, std::string type_name) :
			Service(name, type_name)
		{}

		// Service events

		void onInit();

		bool onFrame();

		bool onDisplay(Event* ev, Unit* unit);

		// factory function
		static Unit* createType(std::string name, std::string type);

	};
}
#endif // !RHEA_GUI_SERVICE
