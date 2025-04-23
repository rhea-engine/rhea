#ifndef RHEA_MODULESYSTEM_H
#define RHEA_MODULESYSTEM_H

namespace Rhea
{
    class Application;

    class IFabricModule {
    public:
        virtual void Register(Application* app) = 0;
        virtual const char* getName() const = 0;
        virtual ~IFabricModule() = default;
    };
}

#endif // !RHEA_MODULESYSTEM_H
