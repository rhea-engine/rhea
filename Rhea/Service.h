#ifndef RHEA_SERVICE_H
#define RHEA_SERVICE_H

#include <string>

#include "Unit.h"

namespace Rhea
{
	class Application;

	class Service : public Unit
	{
		MECLASS(Service, Unit)
	public:
		Service(std::string service_name, std::string type_name) :
			Unit(service_name, type_name)
		{}
		Service(std::string service_name) :
			Service(service_name, "//meworla/service")
		{}

		//  events
		
		// init event
		virtual void onInit() {};

		// every frame
		virtual bool onFrame(float delta) { return false; };
		virtual bool onFrameFinal(float delta) { return false; };

	protected:
		
	};
}

#endif // !RHEA_SERVICE_H
