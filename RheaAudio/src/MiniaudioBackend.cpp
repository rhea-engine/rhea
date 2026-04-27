#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"

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
        bool decoder_initialized = false;
        bool initialized = false;
    };

    class MiniaudioBackend::Impl
    {
    public:
        ma_engine engine;
        bool initialized = false;

        std::unordered_map<AudioResource*, MiniaudioSoundData*> sounds;
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

    bool MiniaudioBackend::load(AudioResource* resource)
    {
        if (!impl->initialized || resource == nullptr)
            return false;

        if (impl->sounds.find(resource) != impl->sounds.end())
            return true;

        auto* data = new MiniaudioSoundData();

        ma_uint32 flags = 0;

        if (resource->getLoadMode() == AudioResource::DECODE_FULLY)
            flags |= MA_SOUND_FLAG_DECODE;

        if (resource->getData())
        {      
            ma_decoder_config decoderConfig = ma_decoder_config_init(
                ma_format_f32,
                0,
                0
            );

            ma_result result = ma_decoder_init_memory(
				resource->getData(),
				resource->getSize(),
                &decoderConfig,
                &data->decoder
            );

            if (result != MA_SUCCESS) {
                LOG(LOG_ERROR) << "MiniaudioBackend::load: Failed to initialize decoder from memory";
                delete data;
                return false;
            }
            data->decoder_initialized = true;

            result = ma_sound_init_from_data_source(
                &impl->engine,
                &data->decoder,
                flags,
                nullptr,
                &data->sound
			);

            if (result != MA_SUCCESS)
            {
                LOG(LOG_ERROR) << "MiniaudioBackend::load: Failed to initialize audio file from memory";
                ma_decoder_uninit(&data->decoder);
                delete data;
                return false;
            }
        }
        else
        {

            const std::string path = resource->getPath(); // TODO: handle path resolution and base paths properly

            ma_result result = ma_sound_init_from_file(
                &impl->engine,
                path.c_str(),
                flags,
                nullptr,
                nullptr,
                &data->sound
            );

            if (result != MA_SUCCESS)
            {
                LOG(LOG_ERROR) << "MiniaudioBackend::load: Failed to load audio file: " << path;
                delete data;
                return false;
            }
        }

        data->initialized = true;

        ma_uint64 lengthFrames = 0;
        ma_sound_get_length_in_pcm_frames(&data->sound, &lengthFrames);

        ma_uint32 sampleRate = ma_engine_get_sample_rate(&impl->engine);

		DLOG(LOG_DEBUG) << "Loaded audio resource: " << resource->getPathName() << " (length: " << lengthFrames << " frames, sample rate: " << sampleRate << ")";

        resource->sampleRate = static_cast<int>(sampleRate);
        resource->durationSeconds =
            sampleRate > 0
            ? static_cast<float>(lengthFrames) / static_cast<float>(sampleRate)
            : 0.0f;

        impl->sounds[resource] = data;
        return true;
    }

    void MiniaudioBackend::unload(AudioResource* resource)
    {
        if (resource == nullptr)
            return;

        auto it = impl->sounds.find(resource);
        if (it == impl->sounds.end())
            return;

        MiniaudioSoundData* data = it->second;

        if (data && data->initialized)
            ma_sound_uninit(&data->sound);
        if (data && data->decoder_initialized)
            ma_decoder_uninit(&data->decoder);

        delete data;
        impl->sounds.erase(it);
    }

    void MiniaudioBackend::play(AudioResource* resource, float volume, bool loop)
    {
        if (!impl->initialized || resource == nullptr)
            return;

        if (!load(resource))
            return;

        MiniaudioSoundData* data = impl->sounds[resource];

        ma_sound_seek_to_pcm_frame(&data->sound, 0);
        ma_sound_set_volume(&data->sound, volume);
        ma_sound_set_looping(&data->sound, loop ? MA_TRUE : MA_FALSE);

        ma_result result = ma_sound_start(&data->sound);
        if (result != MA_SUCCESS)
        {
            LOG(LOG_ERROR) << "MiniaudioBackend::play: Failed to play audio";
        }
    }

    void MiniaudioBackend::stop(AudioResource* resource)
    {
        if (resource == nullptr)
            return;

        auto it = impl->sounds.find(resource);
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

    void MiniaudioBackend::setMasterVolume(float volume)
    {
        if (!impl->initialized)
            return;

        ma_engine_set_volume(&impl->engine, volume);
    }
}