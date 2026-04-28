#ifndef RHEAAUDIO_AUDIOSERVICE_H
#define RHEAAUDIO_AUDIOSERVICE_H

#include <unordered_map>
#include <memory>

#include <Rhea/Service.h>
#include "IAudioBackend.h"
#include "BaseAudioTypes.h"

namespace rhea::audio
{
    class AudioService : public rhea::Service
    {
        RECLASS(AudioService, rhea::Service, "//meworla/audioservice")
    public:
        AudioService(std::string name, std::string type_name) : rhea::Service(std::move(name), std::move(type_name)) {}

        bool initialize();
        void shutdown();

        AudioBusHandle createBus(const std::string& name, const std::string& parent = "Master");
        AudioBusHandle getBus(const std::string& name) const;

        void unload(AudioResource* resource);

        AudioSoundHandle play(AudioResource* resource, AudioBusHandle bus, bool loop = false);
        AudioSoundHandle play(AudioResource* resource, AudioBusHandle bus, bool loop, float volume);
        AudioSoundHandle play(AudioResource* resource, const std::string& bus_name, bool loop = false);
        AudioSoundHandle play(AudioResource* resource, const std::string& bus_name, bool loop, float volume);
        void stopAll();
		void stop(AudioSoundHandle handle);

        void pause(AudioSoundHandle sound);
        void resume(AudioSoundHandle sound);

        void pauseBus(AudioBusHandle bus);
        void resumeBus(AudioBusHandle bus);

        void setMasterVolume(float volume);
        void setBusVolume(const std::string& name, float volume);

    private:
        std::unique_ptr<IAudioBackend> m_backend;
        std::unordered_map<std::string, AudioBusHandle> m_busNameToHandle;
    };
}
#endif // !RHEAAUDIO_AUDIOSERVICE_H
