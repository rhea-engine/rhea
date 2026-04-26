#ifndef RHEA_AUDIO_RESOURCE_H
#define RHEA_AUDIO_RESOURCE_H

#include <Rhea/Resources/Resource.h>

namespace rhea::audio
{
    class AudioResource : public Resource
    {
        RECLASS(AudioResource, Resource, "//meworla/audio")

    public:
        AudioResource(std::string basename, std::string type_name)
            : Resource(basename, type_name)
        {
        }

        ~AudioResource();

        enum AudioLoadMode
        {
            DECODE_FULLY = 0,
            STREAM = 1
        };

        void setup(Application* app) { Unit::setup(app); }

        void setup(
            Application* app,
            std::unordered_map<std::string, std::string> attributes,
            std::unordered_map<std::string, Variant> values
        );

        std::vector<Unit::Export> getReferences();

        void ready();

        virtual AudioLoadMode getLoadMode() const { return DECODE_FULLY; }

        int channels = 0;
        int sampleRate = 0;
        float durationSeconds = 0.0f;
    };
}

#endif // !RHEA_AUDIO_RESOURCE_H