#ifndef RHEA_H
#define RHEA_H

#include "Poseidon/Core.h"
#include "Orion/Orion.h"
#include "Orion/ResourceReadyInterface.h"

using Poseidon::Core;

namespace Rhea
{	
	class Application : public Orion::ResourceReadyInterface
	{
	protected:
		Core* core = 0;
		Orion::Orion* m_fs = 0;
		std::string m_appname = "";
	public:
		void init();
		void setAppname(std::string appname) { m_appname = appname; };
		Core* get_core() { return core; };
		virtual void setup();
		virtual int loop();
		virtual void close();
		int loadScene(const std::string resourceId);
		void onResourceReady(Orion::Resource*);
		virtual void onResourceFailed(Orion::ResourceRequest::Error reason);
	};
}
#endif // !RHEA_H
