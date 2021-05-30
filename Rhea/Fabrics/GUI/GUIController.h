#ifndef RHEA_GUI_CONTROLLER_H
#define RHEA_GUI_CONTROLLER_H

#include "Resource.h"

namespace Rhea
{
	class Widget;

	class GUIController : public Resource
	{
	public:
		GUIController(std::string name) :
			GUIController(name, "//meworla/mwgui/controller")
		{}
		GUIController(std::string name, std::string type_name) :	
			m_widget(nullptr),
			m_target(nullptr),
			m_captured(nullptr),
			m_mx(0),
			m_my(0),
			Resource(name, type_name)
		{}

		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// ready event handler
		void ready();

		// input event handler
		bool onInput(Event* ev, Unit* unit);

		// factory function
		static Unit* createType(std::string name, std::string type);

		// gui fabric function
		static void Register(Application* app);

	protected:
		Widget* m_widget;

		// the currently active widget that receives user input
		Widget* m_target;
		// the current hover widget
		Widget* m_captured;

		// last mouse position
		int m_mx;
		int m_my;
	};
}

#endif // !RHEA_GUI_CONTROLLER_H
