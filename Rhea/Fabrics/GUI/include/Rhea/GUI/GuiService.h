/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GUI_SERVICE
#define RHEA_GUI_SERVICE

#include <Rhea/Service.h>

namespace rhea
{
	class GUIController;

	class GuiService : public Service
	{
		MECLASS(GuiService, Service)
	public:
		GuiService(std::string name, std::string type_name) :
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
