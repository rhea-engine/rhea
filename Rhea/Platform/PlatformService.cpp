/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Psio/Psio.h>
#include <Psio/Net/Network.h>
#include <Rhea/Application.h>
#include <Rhea/ConfigFile.h>

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
		auto* config = m_app->getConfig();
		std::string cert_config = config->getValueString("ssl.cacert", "");
		if (!cert_config.empty()) {
			m_psio->getNet()->SetSetting("ssl_root_ca", cert_config);
		}
	}
}
