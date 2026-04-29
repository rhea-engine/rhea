/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEAAUDIO_IBACKEND_H
#define RHEAAUDIO_IBACKEND_H    

#include <RheaAudio/AudioResource.h>
#include <RheaAudio/BaseAudioTypes.h>

namespace rhea::audio
{
    class IAudioBackend
    {
    public:
        virtual ~IAudioBackend() = default;

        virtual bool initialize() = 0;
        virtual void shutdown() = 0;

        virtual AudioBusHandle createBus(AudioBusHandle parent) = 0;
        virtual void destroyBus(AudioBusHandle bus) = 0;
       
        virtual AudioSoundHandle load(AudioResource* resource, AudioBusHandle bus) = 0;
        virtual void unload(AudioResource* resource) = 0;

        virtual AudioSoundHandle play(AudioResource* resource, AudioBusHandle bus, float volume, bool loop) = 0;
		virtual void stop(AudioSoundHandle sound) = 0;
        virtual void stopAll() = 0;

        virtual void pause(AudioSoundHandle sound) = 0;
		virtual void resume(AudioSoundHandle sound) = 0;

        virtual void pauseBus(AudioBusHandle bus) = 0;
        virtual void resumeBus(AudioBusHandle bus) = 0;

        virtual void setMasterVolume(float volume) = 0;
        virtual void setBusVolume(AudioBusHandle bus, float volume) = 0;
    };
}
#endif // !RHEAAUDIO_IBACKEND_H