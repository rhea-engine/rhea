#ifndef RHEA_MODULESYSTEM_H
#define RHEA_MODULESYSTEM_H

namespace Rhea
{
    class Application;

    class IFabric {
    public:
        virtual void Register(Application* app) = 0;
        virtual const char* getName() const = 0;
        virtual ~IFabric() = default;
    };
}

#endif // !RHEA_MODULESYSTEM_H
