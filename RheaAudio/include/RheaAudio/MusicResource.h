/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

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

        void ready() override;   
    };
}

#endif // !RHEA_MUSIC_RESOURCE_H