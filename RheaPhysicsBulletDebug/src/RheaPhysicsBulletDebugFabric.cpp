/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <RheaPhysicsBulletDebug/RheaPhysicsBulletDebugFabric.h>
#include <Rhea/Physics/PhysicsManagerService.h>
#include <RheaPhysicsBullet/RheaPhysicsBulletService.h>
#include "BtDebugDraw.h"
#include <Rhea/Application.h>
#include <Poseidon/Poseidon.h>
#include <Rhea/UnitTree.h>
#include <RheaClient/Graphics/GraphicsService.h>

namespace rhea::physics::bullet::debug
{
    void RheaPhysicsBulletDebugFabric::Register(rhea::Application* app)
    {
        auto* mgr = doCast<rhea::PhysicsManagerService>(app->getService("physics_mgr"));
        if (!mgr) return;

        auto* svc = mgr->getPhysicsService();
        auto* bullet_svc = doCast<BulletPhysicsService>(svc);
        if (!bullet_svc) return; // bullet backend not active

        mgr->addBackendHook([app](rhea::PhysicsService* svc) {
            auto* bullet_svc = doCast<BulletPhysicsService>(svc);
            if (!bullet_svc) return;            
            bullet_svc->setDrawerFactory([app, bullet_svc]() -> rhea::IPhysicsDebugDraw* {
                auto* graphics = doCast<rhea::client::GraphicsService>(
                    app->getService("graphics")
                );
                if (!graphics) return nullptr;

                auto* core = graphics->getEngine();
                if (!core) return nullptr;

                return new BulletDebugDraw(app, bullet_svc->getWorld(), core);
            });
        });
    }
}