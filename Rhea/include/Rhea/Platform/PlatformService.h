#ifndef RHEA_PLATFORMSERVICE_H
#define RHEA_PLATFORMSERVICE_H

#include <memory>

#include <Rhea/Service.h>

namespace Psio
{
	class Engine;
}

namespace Rhea
{
	class PlatformService : public Service
	{
		MECLASS(PlatformService, Service)
	public:
		PlatformService(std::string name, std::string type_name);

		Psio::Engine* getEngine() { if (!m_psio) createEngine(); return m_psio.get(); }

		// Service functions
		void onInit();
	
	protected:
		Psio::Engine* createEngine();
		std::unique_ptr<Psio::Engine> m_psio;
	};
}
#endif // !RHEA_PLATFORMSERVICE_H
