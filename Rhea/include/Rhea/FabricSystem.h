#ifndef RHEA_FABRICSYSTEM_H
#define RHEA_FABRICSYSTEM_H

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

#endif // !RHEA_FABRICSYSTEM_H
