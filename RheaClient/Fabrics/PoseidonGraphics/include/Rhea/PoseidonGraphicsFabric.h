#ifndef RHEACLIENT_POSEIDONGRAPHICSFABRIC_H
#define RHEACLIENT_POSEIDONGRAPHICSFABRIC_H

#include <Rhea/FabricSystem.h>

namespace rhea
{
    class Application;
}

namespace rhea::client {

    class PoseidonGraphicsFabric : public rhea::IFabric
    {
    public:
        void Register(rhea::Application* app) override;
        const char* getName() const override { return "GraphicsPoseidon"; }
    };
}

#endif // !RHEACLIENT_POSEIDONGRAPHICSFABRIC_H
