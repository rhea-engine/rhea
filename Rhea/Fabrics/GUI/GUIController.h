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
			Resource(name, type_name)
		{}

		// setup from strings
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// ready event handler
		void ready();

		// load handler
		void load(Orion::ResourceRequest* req, ReadyHandler handler);

		// factory function
		static Unit* createType(std::string name, std::string type);

		// gui fabric function
		static void Register(Application* app);

	protected:
		Widget* m_widget;
	};
}

#endif // !RHEA_GUI_CONTROLLER_H
