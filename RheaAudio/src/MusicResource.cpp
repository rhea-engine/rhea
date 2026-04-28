#include <RheaAudio/MusicResource.h>
#include <Psio/Log.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <RheaAudio/AudioService.h>

namespace rhea::audio
{
	void MusicResource::ready()
	{
		AudioResource::ready();

		if (m_status >= Status::READY)
			return;
		m_status = Status::READY;
		Unit::ready();

		auto path_it = m_values.find("path");
		if (!m_buf && path_it != m_values.end()) {
			m_proxy_resource = doCast<Resource>(m_app->getUnitTree()->find(path_it->second.toString(), m_attributes["basepath"], false));
			if (m_proxy_resource && m_proxy_resource->getSize()) {			
				m_status = Status::LOADED;
			}
			else if (m_buf && m_buflen) {
				// set in another way?
				m_status = Status::LOADED;
			}

			if (m_status == Status::LOADED) {

				AudioService* audio_srv = doCast<AudioService>(m_app->getService("audio"));

				if (!audio_srv->load(this))
				{
					LOG(LOG_ERROR) << "MusicResource::ready: Failed to load music " << getPathName() << " audio service failed to load resource.";
					return;
				}
			}
			else {
				LOG(LOG_ERROR) << "MusicResource::ready: Failed to load music " << getPathName() << " music not found or not valid";
			}
		}
	}
}