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

        bool load(AudioResource* resource) override;
        void unload(AudioResource* resource) override;

        void play(AudioResource* resource, float volume, bool loop) override;
        void stop(AudioResource* resource) override;
        void stopAll() override;

        void setMasterVolume(float volume) override;

    private:
        class Impl;
        Impl* impl = nullptr;
    };
}