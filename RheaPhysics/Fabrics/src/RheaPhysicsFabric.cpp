#include <RheaPhysics/RheaPhysicsFabric.h>
#include <Rhea/Physics/PhysicsManagerService.h>
#include "../../src/NullPhysicsService.h"
#include <RheaPhysics/PhysicsComponent.h>

#include <Rhea/Core/Item2D.h>

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
        mgr->registerBackend("none", [](rhea::Application* app)->std::unique_ptr<rhea::PhysicsService> {
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
	}
}