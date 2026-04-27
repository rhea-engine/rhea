#include <RheaAudio/AudioService.h>

#include <Rhea/Application.h>

#include <RheaAudio/MiniaudioBackend.h>

namespace rhea::audio
{
	bool AudioService::initialize()
	{
		m_backend = std::make_unique<MiniaudioBackend>();
		if (!m_backend->initialize()) {
			return false;
		}
		return true;
	}

	void AudioService::shutdown()
	{
		if (m_backend) {
			m_backend->shutdown();
			m_backend.reset();
		}
	}

	bool AudioService::load(AudioResource* resource)
	{
		if (m_backend) {
			return m_backend->load(resource);
		}
		return false;
	}

	void AudioService::play(AudioResource* resource)
	{
		if (m_backend) {
			m_backend->play(resource, 1.0f, false);
		}
	}

	void AudioService::play(AudioResource* resource, float volume, bool loop)
	{
		if (m_backend) {
			m_backend->play(resource, volume, loop);
		}
	}

	void AudioService::stopAll()
	{
		if (m_backend) {
			m_backend->stopAll();
		}
	}

	void AudioService::setMasterVolume(float volume)
	{
		if (m_backend) {
			m_backend->setMasterVolume(volume);
		}
	}

	void AudioService::setMusicVolume(float volume)
	{
	}

	void AudioService::setSfxVolume(float volume)
	{
	}


}