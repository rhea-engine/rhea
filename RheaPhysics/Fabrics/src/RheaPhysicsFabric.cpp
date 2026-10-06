/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <RheaPhysics/RheaPhysicsFabric.h>
#include <Rhea/Physics/PhysicsManagerService.h>
#include "../../src/NullPhysicsService.h"
#include <RheaPhysics/PhysicsComponent.h>

#include <Rhea/Core/Item2D.h>
#include <Rhea/Core/Camera.h>
#include <Rhea/Core/Light.h>

namespace rhea::physics
{
	void RheaPhysicsFabric::Register(rhea::Application* app)
	{
        PhysicsComponent::_register_PhysicsComponent();

		// register physics service
		auto* mgr = static_cast<rhea::PhysicsManagerService*>(app->getService("physics_mgr"));
        if (!mgr) {
            mgr = new rhea::PhysicsManagerService("PhysicsManagerService", "//meworla/physicsmanagerservice");
            mgr->setup(app);           // if you have setup()
            app->registerService("PhysicsManager", mgr);
        }

        // register null backend
        mgr->registerBackend("null", [](rhea::Application* app)->std::unique_ptr<rhea::PhysicsService> {
            return std::make_unique<rhea::physics::NullPhysicsService>("PhysicsService", "//meworla/service");			
            });

        // register a default for all types
        rhea::PhysicsBodyDesc rigidbody;
        rigidbody.kind = "Meworla/RheaPhysics:RigidBody";
        mgr->registerBodyForType(rhea::Item::staticType(), rigidbody);

        // opt-out: no creator - move to rheaclient/fabrics
        rhea::PhysicsBodyDesc none;
        none.kind = ""; // means disabled
        mgr->registerBodyForType(rhea::Item2D::staticType(), none);
        mgr->registerBodyForType(rhea::Camera::staticType(), none);
        mgr->registerBodyForType(rhea::Light::staticType(), none);
	}
}