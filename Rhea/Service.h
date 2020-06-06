#ifndef RHEA_SERVICE_H
#define RHEA_SERVICE_H

#include <string>

#include "Unit.h"

namespace Rhea
{
	class Application;

	class Service : public Unit
	{
	public:
		Service(Application* app, std::string service_name) :
			Unit(service_name, "//meworla/service")
		{
			m_app = app;
		}

		//  events
		
		// init event
		virtual void onInit() {};

		// every frame
		virtual bool onFrame() { return false; };

	protected:
		
	};
}

#endif // !RHEA_SERVICE_H
