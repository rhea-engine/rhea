#include <Psio/Psio.h>

#include "PlatformService.h"

namespace Rhea
{
	PlatformService::PlatformService(Application* app) :
		Service(app, "platform")
	{
		m_psio = std::make_unique<Psio::Engine>();
	}

	void PlatformService::onInit()
	{
	}

}
