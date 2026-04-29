/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

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
