#ifndef RHEA_PLATFORMSERVICE_H
#define RHEA_PLATFORMSERVICE_H

#include <memory>

#include <Rhea/Service.h>

namespace psio
{
	class Engine;
}

namespace rhea
{
	class PlatformService : public Service
	{
		MECLASS(PlatformService, Service)
	public:
		PlatformService(std::string name, std::string type_name);

		psio::Engine* getEngine() { if (!m_psio) createEngine(); return m_psio.get(); }

		// Service functions
		void onInit();
	
	protected:
		psio::Engine* createEngine();
		std::unique_ptr<psio::Engine> m_psio;
	};
}
#endif // !RHEA_PLATFORMSERVICE_H
