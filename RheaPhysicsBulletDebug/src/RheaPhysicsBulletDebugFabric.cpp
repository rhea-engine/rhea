/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <RheaPhysicsBulletDebug/RheaPhysicsBulletDebugFabric.h>
#include <RheaPhysicsBulletDebug/BtDebugDraw.h>
#include <Rhea/Physics/PhysicsManagerService.h>
#include <RheaPhysicsBullet/RheaPhysicsBulletService.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>

namespace rhea::physics::bullet::debug
{
    void RheaPhysicsBulletDebugFabric::Register(rhea::Application* app)
    {
        auto* mgr = doCast<rhea::PhysicsManagerService>(app->getService("physics_mgr"));
        if (!mgr) return;

        BulletDebugDraw::_register_BulletDebugDraw();
       
        mgr->addBackendHook([app](rhea::PhysicsService* svc) {
            auto* bullet_svc = doCast<BulletPhysicsService>(svc);
            if (!bullet_svc) return;
            bullet_svc->setDrawerFactory([app, bullet_svc]() -> rhea::IPhysicsDebugDraw* {
                BulletDebugDraw* drawer = new BulletDebugDraw("btDebugDrawer");
                drawer->setup(app, bullet_svc);
                return drawer;
                });
            });
    }
}