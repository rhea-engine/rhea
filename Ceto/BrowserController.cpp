#include "BrowserController.h"
#include <Rhea/Core/WorldService.h>
#include <Psio/Log.h>

void BrowserController::openScene(std::string scene_name, bool mark_preferences)
{
	auto* worlds = doCast<rhea::WorldService>(m_app->getService("world"));

	if (m_loading_handler)
		m_loading_handler(true);

	worlds->openWorld(scene_name, [this, scene_name, mark_preferences, worlds](rhea::ErrorReport error) {

		if (m_loading_handler)
			m_loading_handler(false);

		if (error.isError()) {
			LOG(LOG_ERROR) << "Ceto: Could not open scene: " << scene_name;

			if (error.isCanceled())
				return;
			if (m_error_handler)
				m_error_handler(error);
			return;
		}

		for (auto open_handler : m_opened_handlers) {
			open_handler(scene_name, mark_preferences);
		}
		});
}