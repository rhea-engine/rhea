#include <Psio/Psio.h>

#include "PlatformService.h"

namespace Rhea
{
	PlatformService::PlatformService(std::string name, std::string type_name) :
		Service("Platform", type_name)
	{
		m_psio = std::make_unique<Psio::Engine>();
	}

	void PlatformService::onInit()
	{
	}

	// factory function
	Unit* PlatformService::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new PlatformService(name, type);
		return ret;
	}
}
