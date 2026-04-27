#ifndef RHEA_MUSIC_RESOURCE_H
#define RHEA_MUSIC_RESOURCE_H

#include <RheaAudio/AudioResource.h>

namespace rhea::audio
{
    class MusicResource : public AudioResource
    {
        RECLASS(MusicResource, AudioResource, "//meworla/music")

    public:
        MusicResource(std::string basename, std::string type_name)
            : AudioResource(basename, type_name)
        {
        }

        AudioLoadMode getLoadMode() const override {
            return STREAM;
        }

        std::vector<Unit::Export> getReferences();
    };
}

#endif // !RHEA_MUSIC_RESOURCE_H