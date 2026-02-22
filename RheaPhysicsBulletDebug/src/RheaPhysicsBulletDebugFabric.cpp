#include <RheaPhysicsBulletDebug/RheaPhysicsBulletDebugFabric.h>
#include <Rhea/Physics/PhysicsManagerService.h>
#include <RheaPhysicsBullet/RheaPhysicsBulletService.h>
#include "BtDebugDraw.h"
#include <Rhea/Application.h>
#include <Poseidon.h>
#include <Rhea/UnitTree.h>

namespace Rhea
{
    void RheaPhysicsBulletDebugFabric::Register(Application* app)
    {
        auto* mgr = doCast<PhysicsManagerService>(app->getService("physics_mgr"));
        if (!mgr) return;

        auto* svc = mgr->getPhysicsService();
        auto* bullet_svc = doCast<BulletPhysicsService>(svc);
        if (!bullet_svc) return; // bullet backend not active

        mgr->addBackendHook([app](PhysicsService* svc) {
            auto* bullet_svc = doCast<BulletPhysicsService>(svc);
            if (!bullet_svc) return;            
            bullet_svc->setDrawerFactory([app, bullet_svc]() -> IPhysicsDebugDraw* {
                Poseidon::Application* p_app = (Poseidon::Application*)app;
                auto* dbg = new BulletDebugDraw(app, bullet_svc->getWorld(), p_app->get_core());
                return (IPhysicsDebugDraw*)dbg;
            });
        });
    }
}