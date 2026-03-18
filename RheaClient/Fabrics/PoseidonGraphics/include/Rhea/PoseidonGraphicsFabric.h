#ifndef RHEACLIENT_POSEIDONGRAPHICSFABRIC_H
#define RHEACLIENT_POSEIDONGRAPHICSFABRIC_H

#include <Rhea/FabricSystem.h>

namespace Rhea
{
    class Application;

    class PoseidonGraphicsFabric : public Rhea::IFabric
    {
    public:
        void Register(Rhea::Application* app) override;
        const char* getName() const override { return "GraphicsPoseidon"; }
    };
}

#endif // !RHEACLIENT_POSEIDONGRAPHICSFABRIC_H
