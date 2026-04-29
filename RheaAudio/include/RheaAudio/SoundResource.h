/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_SOUND_RESOURCE_H
#define RHEA_SOUND_RESOURCE_H

#include <RheaAudio/AudioResource.h>

namespace rhea::audio
{
    class SoundResource : public AudioResource
    {
        RECLASS(SoundResource, AudioResource, "//meworla/sound")

    public:
        SoundResource(std::string basename, std::string type_name)
            : AudioResource(basename, type_name)
        {
        }

        AudioLoadMode getLoadMode() const override {
            return DECODE_FULLY;
        }

        void ready() override;
    };
}

#endif // !RHEA_SOUND_RESOURCE_H