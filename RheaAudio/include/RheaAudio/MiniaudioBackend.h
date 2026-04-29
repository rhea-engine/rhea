/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#pragma once

#include <RheaAudio/IAudioBackend.h>

namespace rhea::audio
{
    class MiniaudioBackend : public IAudioBackend
    {
    public:
        MiniaudioBackend();
        ~MiniaudioBackend() override;

        bool initialize() override;
        void shutdown() override;

        AudioBusHandle createBus(AudioBusHandle parent);
        void destroyBus(AudioBusHandle bus);        

        AudioSoundHandle load(AudioResource* resource, AudioBusHandle bus) override;
        void unload(AudioResource* resource) override;

        AudioSoundHandle play(AudioResource* resource, AudioBusHandle bus, float volume, bool loop) override;
        void stop(AudioSoundHandle sound) override;
        void stopAll() override;

        void pause(AudioSoundHandle sound) override;
        void resume(AudioSoundHandle sound) override;

        void pauseBus(AudioBusHandle bus) override;
        void resumeBus(AudioBusHandle bus) override;

        void setMasterVolume(float volume) override;
        void setBusVolume(AudioBusHandle bus, float volume);

    private:
        class Impl;
        Impl* impl = nullptr;       
    };
}