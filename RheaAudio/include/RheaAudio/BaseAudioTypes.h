/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEAAUDIO_BASEAUDIOTYPES_H
#define RHEAAUDIO_BASEAUDIOTYPES_H

#include <cstdint>
#include <string>

namespace rhea::audio
{
    struct AudioBusHandle
    {
        uint32_t id = 0;

        bool operator==(const AudioBusHandle& other) const
        {
            return id == other.id;
        }

        bool operator!=(const AudioBusHandle& other) const
        {
            return id != other.id;
        }
    };

    struct AudioBusDesc
    {
        std::string name;
        std::string parent = "Master";
        float volume = 1.0f;
    };

    struct AudioSoundHandle
    {
        uint32_t id = 0;

        bool operator==(const AudioSoundHandle& other) const
        {
            return id == other.id;
        }

        bool operator!=(const AudioSoundHandle& other) const
        {
            return id != other.id;
        }
    };
}

// Hash-Spezialisierungen für std::unordered_map
namespace std
{
    template<>
    struct hash<rhea::audio::AudioBusHandle>
    {
        size_t operator()(const rhea::audio::AudioBusHandle& handle) const noexcept
        {
            return std::hash<uint32_t>{}(handle.id);
        }
    };

    template<>
    struct hash<rhea::audio::AudioSoundHandle>
    {
        size_t operator()(const rhea::audio::AudioSoundHandle& handle) const noexcept
        {
            return std::hash<uint32_t>{}(handle.id);
        }
    };
}

#endif // !RHEAAUDIO_BASEAUDIOTYPES_H
