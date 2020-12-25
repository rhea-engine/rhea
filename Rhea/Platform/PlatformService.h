#ifndef RHEA_PLATFORMSERVICE_H
#define RHEA_PLATFORMSERVICE_H

#include <memory>

#include "../Service.h"

namespace Psio
{
	class Engine;
}

namespace Rhea
{
	class PlatformService : public Service
	{
	public:
		PlatformService(std::string name, std::string type_name);

		Psio::Engine* getEngine() { return m_psio.get(); }

		// Service functions
		void onInit();

		// factory function
		static Unit* createType(std::string name, std::string type);
	
	protected:
		std::unique_ptr<Psio::Engine> m_psio;
	};
}
#endif // !RHEA_PLATFORMSERVICE_H
