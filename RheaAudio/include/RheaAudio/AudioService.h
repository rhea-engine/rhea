#ifndef RHEAAUDIO_AUDIOSERVICE_H
#define RHEAAUDIO_AUDIOSERVICE_H

#include <Rhea/Service.h>
#include "IAudioBackend.h"

namespace rhea::audio
{
    class AudioService : public rhea::Service
    {
        RECLASS(AudioService, rhea::Service, "//meworla/audioservice")
    public:
        AudioService(std::string name, std::string type_name) : rhea::Service(std::move(name), std::move(type_name)) {}

        bool initialize();
        void shutdown();

		bool load(AudioResource* resource);

        void play(AudioResource* resource);
        void play(AudioResource* resource, float volume, bool loop);
        void stopAll();

        void setMasterVolume(float volume);
        void setMusicVolume(float volume);
        void setSfxVolume(float volume);

    private:
        std::unique_ptr<IAudioBackend> m_backend;
    };
}
#endif // !RHEAAUDIO_AUDIOSERVICE_H
