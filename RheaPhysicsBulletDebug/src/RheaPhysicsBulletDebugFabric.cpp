#include <RheaPhysicsBulletDebug/RheaPhysicsBulletDebugFabric.h>
#include <Rhea/Physics/PhysicsManagerService.h>
#include <RheaPhysicsBullet/RheaPhysicsBulletService.h>
#include "BtDebugDraw.h"
#include <Rhea/Application.h>
#include <Poseidon/Poseidon.h>
#include <Rhea/UnitTree.h>

namespace rhea::physics::bullet::debug
{
    void RheaPhysicsBulletDebugFabric::Register(Rhea::Application* app)
    {
        auto* mgr = doCast<Rhea::PhysicsManagerService>(app->getService("physics_mgr"));
        if (!mgr) return;

        auto* svc = mgr->getPhysicsService();
        auto* bullet_svc = doCast<BulletPhysicsService>(svc);
        if (!bullet_svc) return; // bullet backend not active

        mgr->addBackendHook([app](Rhea::PhysicsService* svc) {
            auto* bullet_svc = doCast<BulletPhysicsService>(svc);
            if (!bullet_svc) return;            
            bullet_svc->setDrawerFactory([app, bullet_svc]() -> Rhea::IPhysicsDebugDraw* {
                Poseidon::Application* p_app = (Poseidon::Application*)app;
                auto* dbg = new BulletDebugDraw(app, bullet_svc->getWorld(), p_app->get_core());
                return (Rhea::IPhysicsDebugDraw*)dbg;
            });
        });
    }
}