#ifndef RHEA_SERVICE_H
#define RHEA_SERVICE_H

#include <string>

namespace Rhea
{
	class Application;

	class Service
	{
	public:
		Service(Application* app, std::string service_name) :
			m_app(app),
			m_name(service_name)
		{}

		//  events
		
		// init event
		virtual void onInit() {};

		// every frame
		virtual bool onFrame() { return false; };

	protected:
		std::string m_name;
		Application* m_app;
	};
}

#endif // !RHEA_SERVICE_H
