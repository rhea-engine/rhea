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

		// create a Master bus
		createBus("Master", "");
		return true;
	}

	void AudioService::shutdown()
	{
		if (m_backend) {
			m_backend->shutdown();
			m_backend.reset();
		}
	}

	AudioBusHandle AudioService::createBus(const std::string& name, const std::string& parent)
	{
		if (!m_backend) {
			return AudioBusHandle();
		}

		// if there is no parent or the bus is "Master", parent handle is 0
		AudioBusHandle parent_handle;
		if (name != "Master" && !parent.empty()) {
			parent_handle = getBus(parent);
		}

		AudioBusHandle handle = m_backend->createBus(parent_handle);
		
		// store the mapping from name to handle
		m_busNameToHandle[name] = handle;
		
		return handle;
	}

	AudioBusHandle AudioService::getBus(const std::string& name) const
	{
		auto it = m_busNameToHandle.find(name);
		if (it != m_busNameToHandle.end()) {
			return it->second;
		}
		return AudioBusHandle();
	}

	void AudioService::setBusVolume(const std::string& name, float volume)
	{
		if (!m_backend) {
			return;
		}

		AudioBusHandle handle = getBus(name);
		if (handle.id != 0) {
			m_backend->setBusVolume(handle, volume);
		}
	}

	void AudioService::unload(AudioResource* resource)
	{
		if (m_backend) {
			m_backend->unload(resource);
		}
	}

	AudioSoundHandle AudioService::play(AudioResource* resource, AudioBusHandle bus, bool loop)
	{
		if (m_backend) {
			return m_backend->play(resource, bus, 1.0f, loop);
		}
		return {};
	}

	AudioSoundHandle AudioService::play(AudioResource* resource, AudioBusHandle bus, bool loop, float volume)
	{
		if (m_backend) {
			return m_backend->play(resource, bus, volume, loop);
		}
		return {};
	}

	AudioSoundHandle AudioService::play(AudioResource* resource, const std::string& bus_name, bool loop)
	{
		AudioBusHandle bus = getBus(bus_name);
		return play(resource, bus, loop);
	}

	AudioSoundHandle AudioService::play(AudioResource* resource, const std::string& bus_name, bool loop, float volume)
	{
		AudioBusHandle bus = getBus(bus_name);
		return play(resource, bus, loop, volume);
	}

	void AudioService::stopAll()
	{
		if (m_backend) {
			m_backend->stopAll();
		}
	}

	void AudioService::stop(AudioSoundHandle handle)
	{
		if (m_backend) {
			m_backend->stop(handle);
		}
	}

	void AudioService::pause(AudioSoundHandle sound)
	{
		if (m_backend) {
			m_backend->pause(sound);
		}
	}

	void AudioService::resume(AudioSoundHandle sound)
	{
		if (m_backend) {
			m_backend->resume(sound);
		}
	}

	void AudioService::pauseBus(AudioBusHandle bus)
	{
		if (m_backend) {
			m_backend->pauseBus(bus);
		}
	}

	void AudioService::resumeBus(AudioBusHandle bus)
	{
		if (m_backend) {
			m_backend->resumeBus(bus);
		}
	}

	void AudioService::setMasterVolume(float volume)
	{
		if (m_backend) {
			m_backend->setMasterVolume(volume);
		}
	}
}