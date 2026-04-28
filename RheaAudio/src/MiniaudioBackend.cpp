#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"

#include "miniaudio/extras/decoders/libvorbis/miniaudio_libvorbis.h"

#include <RheaAudio/MiniaudioBackend.h>
#include <RheaAudio/AudioResource.h>

#include <unordered_map>
#include <string>
#include <iostream>
#include <Psio/Log.h>

namespace rhea::audio
{
    struct MiniaudioSoundData
    {
		ma_decoder decoder{};
        ma_sound sound{};

        AudioResource* resource = nullptr;
        AudioBusHandle bus{}; // 0 means "no bus"
        
        bool decoder_initialized = false;
        bool initialized = false;
    };

    struct MiniaudioBus
    {
        ma_sound_group group{};
        bool initialized = false;

        MiniaudioBus() = default;

        MiniaudioBus(const MiniaudioBus&) = delete;
        MiniaudioBus& operator=(const MiniaudioBus&) = delete;

        MiniaudioBus(MiniaudioBus&&) = default;
        MiniaudioBus& operator=(MiniaudioBus&&) = default;
    };

    class MiniaudioBackend::Impl
    {
    public:
        ma_engine engine;
        bool initialized = false;

		uint32_t next_bus_id = 1; // Master begins at 1, 0 is "no bus"
		uint32_t next_sound_id = 1;

        std::unordered_map<uint32_t, MiniaudioBus> buses;
        std::unordered_map<AudioSoundHandle, MiniaudioSoundData*> sounds;
    };

    MiniaudioBackend::MiniaudioBackend()
        : impl(new Impl())
    {
    }

    MiniaudioBackend::~MiniaudioBackend()
    {
        shutdown();
        delete impl;
    }

    bool MiniaudioBackend::initialize()
    {
        if (impl->initialized)
            return true;

        ma_result result = ma_engine_init(nullptr, &impl->engine);
        if (result != MA_SUCCESS)
        {
            LOG(LOG_ERROR) << "MiniaudioBackend::initialize: Failed to initialize miniaudio engine";
            return false;
        }
        ma_engine_get_sample_rate(&impl->engine);
		LOG(LOG_INFO) << "Miniaudio engine initialized with sample rate: " << ma_engine_get_sample_rate(&impl->engine);

        impl->initialized = true;
        return true;
    }

    void MiniaudioBackend::shutdown()
    {
        if (!impl->initialized)
            return;

        for (auto& pair : impl->sounds)
        {
            MiniaudioSoundData* data = pair.second;

            if (data && data->initialized)
                ma_sound_uninit(&data->sound);

            delete data;
        }

        impl->sounds.clear();

        ma_engine_uninit(&impl->engine);
        impl->initialized = false;
    }

    AudioBusHandle MiniaudioBackend::createBus(AudioBusHandle parent)
    {
        uint32_t id = impl->next_bus_id++;
        
        auto [it, inserted] = impl->buses.try_emplace(id, MiniaudioBus{});
        MiniaudioBus& bus = it->second;

        ma_sound_group* parentGroup = nullptr;

        if (parent.id != 0)
        {
            auto parentIt = impl->buses.find(parent.id);
            if (parentIt != impl->buses.end())
                parentGroup = &parentIt->second.group;
        }

        ma_result result = ma_sound_group_init(
            &impl->engine,
            0,
            parentGroup,
            &bus.group
        );
        
        if (result != MA_SUCCESS)
        {
            impl->buses.erase(id);
            return {};
        }

        bus.initialized = true;
        return AudioBusHandle{ id };        
    }

    void MiniaudioBackend::destroyBus(AudioBusHandle bus)
    {
        if (bus.id == 0)
            return;

        auto it = impl->buses.find(bus.id);
        if (it == impl->buses.end())
            return;

        // Stop/remove voices using this bus.
		std::vector<AudioSoundHandle> sound_to_destroy;

        for (auto& [id, sound] : impl->sounds)
        {
            if (sound->bus.id == bus.id)
            {
                ma_sound_stop(&sound->sound);
                if (sound->initialized)
                    ma_sound_uninit(&sound->sound);

                if (sound->decoder_initialized)
                    ma_decoder_uninit(&sound->decoder);

                sound_to_destroy.push_back(id);
            }
        }

        for (AudioSoundHandle id : sound_to_destroy)
            impl->sounds.erase(id);

        MiniaudioBus& busData = it->second;
        
        if (busData.initialized)
        {
            ma_sound_group_uninit(&busData.group);
            busData.initialized = false;
        }

        impl->buses.erase(it);
    }

    void MiniaudioBackend::setBusVolume(AudioBusHandle bus, float volume)
    {
        if (bus.id == 0)
            return;

        auto it = impl->buses.find(bus.id);
        if (it == impl->buses.end())
            return;

        MiniaudioBus& busData = it->second;
        
        if (busData.initialized)
        {
            ma_sound_group_set_volume(&busData.group, volume);
        }
    }

    AudioSoundHandle MiniaudioBackend::load(AudioResource* resource, AudioBusHandle bus)
    {
        if (!impl->initialized || resource == nullptr)
            return {};

        auto* data = new MiniaudioSoundData();
        data->bus.id = bus.id;
		data->resource = resource;

		ma_sound_group* group = nullptr;
        auto it = impl->buses.find(bus.id);
        if (it != impl->buses.end())
            group = &it->second.group;

        ma_uint32 flags = 0;

        if (resource->getLoadMode() == AudioResource::DECODE_FULLY)
            flags |= MA_SOUND_FLAG_DECODE;

        ma_decoding_backend_vtable* customBackends[] =
        {
            ma_decoding_backend_libvorbis
        };

        ma_decoder_config decoderConfig = ma_decoder_config_init(
            ma_format_f32,
            0,
            0
        );
        decoderConfig.ppCustomBackendVTables = customBackends;
        decoderConfig.customBackendCount = sizeof(customBackends) / sizeof(customBackends[0]);

        ma_result result = MA_INVALID_ARGS;
        if (resource->getData())
        {
            result = ma_decoder_init_memory(
                resource->getData(),
                resource->getSize(),
                &decoderConfig,
                &data->decoder
            );
        }
        else {
            const std::string path = resource->getPath(); // TODO: handle path resolution and base paths properly
            result = ma_decoder_init_file(
                path.c_str(),
                &decoderConfig,
                &data->decoder
            );
        }
        if (result != MA_SUCCESS) {
            LOG(LOG_ERROR) << "MiniaudioBackend::load: Failed to initialize decoder from memory";
            delete data;
            return {};
        }
        data->decoder_initialized = true;

        result = ma_sound_init_from_data_source(
            &impl->engine,
            &data->decoder,
            flags,
            group,
            &data->sound
        );

        if (result != MA_SUCCESS)
        {
            LOG(LOG_ERROR) << "MiniaudioBackend::load: Failed to initialize audio file from memory";
            ma_decoder_uninit(&data->decoder);
            delete data;
            return {};
        }

        data->initialized = true;

        ma_uint64 lengthFrames = 0;
        ma_sound_get_length_in_pcm_frames(&data->sound, &lengthFrames);

        ma_uint32 sampleRate = ma_engine_get_sample_rate(&impl->engine);

        //DLOG(LOG_DEBUG) << "Loaded audio resource: " << resource->getPathName() << " (length: " << lengthFrames << " frames, sample rate: " << sampleRate << ")";

        resource->sampleRate = static_cast<int>(sampleRate);
        resource->durationSeconds =
            sampleRate > 0
            ? static_cast<float>(lengthFrames) / static_cast<float>(sampleRate)
            : 0.0f;

		uint32_t sound_id = impl->next_sound_id++;

        impl->sounds[{sound_id}] = data;
        return { sound_id };
    }

    void MiniaudioBackend::unload(AudioResource* resource)
    {
        if (resource == nullptr)
            return;

        // Find all sounds using this resource
        std::vector<AudioSoundHandle> sounds_to_remove;
        
        for (auto& [handle, data] : impl->sounds)
        {
            if (data->resource == resource)
            {
                sounds_to_remove.push_back(handle);
            }
        }

        // Uninitialize and remove all found sounds
        for (AudioSoundHandle handle : sounds_to_remove)
        {
            auto it = impl->sounds.find(handle);
            if (it == impl->sounds.end())
                continue;

            MiniaudioSoundData* data = it->second;

            if (data && data->initialized)
                ma_sound_uninit(&data->sound);
            if (data && data->decoder_initialized)
                ma_decoder_uninit(&data->decoder);

            delete data;
            impl->sounds.erase(it);
        }
    }

    AudioSoundHandle MiniaudioBackend::play(AudioResource* resource, AudioBusHandle bus, float volume, bool loop)
    {
        if (!impl->initialized || resource == nullptr)
            return {};

		AudioSoundHandle handle = load(resource, bus);
        if (!handle.id)
			return {};

        MiniaudioSoundData* data = impl->sounds[handle];

        ma_sound_seek_to_pcm_frame(&data->sound, 0);
        ma_sound_set_volume(&data->sound, volume);
        ma_sound_set_looping(&data->sound, loop ? MA_TRUE : MA_FALSE);

        ma_result result = ma_sound_start(&data->sound);
        if (result != MA_SUCCESS)
        {
            LOG(LOG_ERROR) << "MiniaudioBackend::play: Failed to play audio";
			return {};
        }
		return handle;
    }

    void MiniaudioBackend::stop(AudioSoundHandle sound)
    {
        if (!sound.id)
            return;

        auto it = impl->sounds.find(sound);
        if (it == impl->sounds.end())
            return;

        ma_sound_stop(&it->second->sound);
        ma_sound_seek_to_pcm_frame(&it->second->sound, 0);
    }

    void MiniaudioBackend::stopAll()
    {
        for (auto& pair : impl->sounds)
        {
            MiniaudioSoundData* data = pair.second;

            if (data && data->initialized)
            {
                ma_sound_stop(&data->sound);
                ma_sound_seek_to_pcm_frame(&data->sound, 0);
            }
        }
    }

    void MiniaudioBackend::pause(AudioSoundHandle sound)
    {
        auto it = impl->sounds.find(sound);
        if (it == impl->sounds.end())
            return;

        ma_sound_stop(&it->second->sound);
    }

    void MiniaudioBackend::resume(AudioSoundHandle sound)
    {
        auto it = impl->sounds.find(sound);
        if (it == impl->sounds.end())
            return;

        ma_sound_start(&it->second->sound);
    }

    void MiniaudioBackend::pauseBus(AudioBusHandle bus)
    {
        for (auto& [id, sound] : impl->sounds)
        {
            if (sound->bus.id == bus.id && sound->initialized)
                ma_sound_stop(&sound->sound);
        }
    }

    void MiniaudioBackend::resumeBus(AudioBusHandle bus)
    {
        for (auto& [id, sound] : impl->sounds)
        {
            if (sound->bus.id == bus.id && sound->initialized)
                ma_sound_start(&sound->sound);
		}
    }

    void MiniaudioBackend::setMasterVolume(float volume)
    {
        if (!impl->initialized)
            return;

        ma_engine_set_volume(&impl->engine, volume);
    }
}