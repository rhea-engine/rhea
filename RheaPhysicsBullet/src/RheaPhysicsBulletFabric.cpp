#include <RheaPhysicsBullet/RheaPhysicsBulletFabric.h>
#include <Rhea/Physics/PhysicsManagerService.h>
#include <RheaPhysicsBullet/RheaPhysicsBulletService.h>

void Rhea::RheaPhysicsBulletFabric::Register(Application* app)
{
    auto* mgr = static_cast<PhysicsManagerService*>(app->getService("physics_mgr"));
    if (!mgr) return; // or create it, but better to require RheaPhysics fabric

    mgr->registerBackend("world", [](Application* app) {
        return std::make_unique<BulletPhysicsService>("BulletPhysicsService", "//meworla/service");
        });
}