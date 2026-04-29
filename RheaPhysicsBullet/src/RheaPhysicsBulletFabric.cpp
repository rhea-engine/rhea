/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <RheaPhysicsBullet/RheaPhysicsBulletFabric.h>
#include <Rhea/Physics/PhysicsManagerService.h>
#include <RheaPhysicsBullet/RheaPhysicsBulletService.h>

void rhea::physics::bullet::RheaPhysicsBulletFabric::Register(rhea::Application* app)
{
    auto* mgr = static_cast<rhea::PhysicsManagerService*>(app->getService("physics_mgr"));
    if (!mgr) return; // or create it, but better to require RheaPhysics fabric

    mgr->registerBackend("world", [](rhea::Application* app) {
        return std::make_unique<rhea::physics::bullet::BulletPhysicsService>("BulletPhysicsService", "//meworla/service");
        });
}