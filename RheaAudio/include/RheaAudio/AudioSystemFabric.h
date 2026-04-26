#ifndef RHEAAUDIO_FABRICS_H
#define RHEAAUDIO_FABRICS_H

#include <Rhea/FabricSystem.h>

namespace rhea
{
    class Application;
}

namespace rhea::audio {

    class AudioSystemFabric : public rhea::IFabric
    {
    public:
        void Register(rhea::Application* app) override;
        const char* getName() const override { return "Audio"; }
    };
}


#endif // !RHEAAUDIO_FABRICS_H
