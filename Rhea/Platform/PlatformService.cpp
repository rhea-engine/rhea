#include <Psio/Psio.h>
#include <Psio/Net/Network.h>

#include "Rhea/Platform/PlatformService.h"

#include <algorithm>
#include <cctype>
#include <string>

namespace rhea
{
	PlatformService::PlatformService(std::string name, std::string type_name) :
		Service("Platform", type_name)
	{
	}

	psio::Engine* PlatformService::createEngine()
	{
		// check requested backend
		std::string backend = m_values["backend"].toString();
		std::transform(backend.begin(), backend.end(), backend.begin(),
			[](unsigned char c) { return std::tolower(c); });
		if (backend == "sdl") {
			m_psio = std::make_unique<psio::Engine>(psio::BackendKind::SDL);
		}
		else if (backend == "win32") {
			m_psio = std::make_unique<psio::Engine>(psio::BackendKind::Win32);
		}
		else if (backend == "unix") {
			m_psio = std::make_unique<psio::Engine>(psio::BackendKind::Unix);
		}
		else {
			// default to auto
			m_psio = std::make_unique<psio::Engine>(psio::BackendKind::Auto);
		}
		return m_psio.get();
	}

	void PlatformService::onInit()
	{
		// initialize ssl settings
		std::string appdir = m_values["AppDir"].toString();
		std::string setting = "ssl_root_ca";
		std::string cert = appdir + "/ssl/cacert.pem";
		m_psio->getNet()->SetSetting(setting, cert);
	}
}
