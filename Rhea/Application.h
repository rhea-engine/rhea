#ifndef RHEA_H
#define RHEA_H

#include "Poseidon/Core.h"
#include "Orion/Orion.h"
#include "Orion/ResourceReadyInterface.h"

namespace Rhea
{	
	class Application : public Orion::ResourceReadyInterface
	{
	protected:
		Poseidon::Core* core = 0;
		Orion::Orion* m_fs = 0;
		std::string m_appname = "";
	public:
		// load and prepare all subsystems - calls setup at  
		void init();
		// set the applications name to appname
		void setAppname(std::string appname) { m_appname = appname; };
		Poseidon::Core* get_core() { return core; };
		// load and set up the resources of the application
		virtual void setup();
		virtual int loop();
		virtual void close();
		int loadScene(const std::string resourceId);
		void onResourceReady(Orion::Resource*);
		virtual void onResourceFailed(Orion::ResourceRequest::Error reason);
	};
}
#endif // !RHEA_H
