#include <Psio/Psio.h>
#include <Psio/Net/Network.h>

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
		// initialize ssl settings
		std::string appdir = m_values["AppDir"];
		std::string setting = "ssl_root_ca";
		std::string cert = appdir + "/ssl/cacert.pem";
		m_psio->getNet()->SetSetting(setting, cert);
	}

	// factory function
	Unit* PlatformService::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new PlatformService(name, type);
		return ret;
	}
}
