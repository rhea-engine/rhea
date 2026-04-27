#include <RheaAudio/SoundResource.h>
#include <Psio/Log.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <RheaAudio/AudioService.h>

namespace rhea::audio
{
	void SoundResource::ready()
	{
		AudioResource::ready();

		if (m_status >= Status::READY)
			return;
		m_status = Status::READY;
		Unit::ready();

		auto path_it = m_values.find("path");
		if (!m_buf && path_it != m_values.end()) {
			Resource* snd = doCast<Resource>(m_app->getUnitTree()->find(path_it->second.toString(), m_attributes["basepath"], false));
			if (snd && snd->getSize()) {
				m_buf = (char*)malloc(snd->getSize());
				if (m_buf && snd->getData() != nullptr) {
					memcpy(m_buf, snd->getData(), snd->getSize());
					m_buflen = snd->getSize();
				}
				else {
					LOG(LOG_ERROR) << "SoundResource::ready: Failed to load sound " << getPathName() << " could not allocate buffers.";
				}
			}
			else {
				LOG(LOG_ERROR) << "SoundResource::ready: Failed to load sound " << getPathName() << " sound not found or not valid";
				//abort();
			}


			if (m_buf && m_buflen) {

				AudioService* audio_srv = doCast<AudioService>(m_app->getService("audio"));

				m_status = Status::LOADED;

				if (!audio_srv->load(this))
					{
					LOG(LOG_ERROR) << "SoundResource::ready: Failed to load sound " << getPathName() << " audio service failed to load resource.";
					return;
				}				
			}
			else {
				LOG(LOG_ERROR) << "SoundResource::ready: Failed to load sound " << getPathName() << " sound not found or not valid";
				//abort();
			}
		}
	}
}