#ifndef RHEAAUDIO_IBACKEND_H
#define RHEAAUDIO_IBACKEND_H    

#include <RheaAudio/AudioResource.h>

namespace rhea::audio
{
    class IAudioBackend
    {
    public:
        virtual ~IAudioBackend() = default;

        virtual bool initialize() = 0;
        virtual void shutdown() = 0;

        virtual bool load(AudioResource* resource) = 0;
        virtual void unload(AudioResource* resource) = 0;

        virtual void play(AudioResource* resource, float volume, bool loop) = 0;
		virtual void stop(AudioResource* resource) = 0;
        virtual void stopAll() = 0;

        virtual void setMasterVolume(float volume) = 0;
    };
}
#endif // !RHEAAUDIO_IBACKEND_H