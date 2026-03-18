#include <RheaPhysics/RheaPhysicsFabric.h>
#include <Rhea/Physics/PhysicsManagerService.h>
#include "../../src/NullPhysicsService.h"
#include <RheaPhysics/PhysicsComponent.h>

#include <Rhea/Core/Item2D.h>

namespace Rhea
{
	void RheaPhysicsFabric::Register(Application* app)
	{
        PhysicsComponent::_register_PhysicsComponent();

		// register physics service
		auto* mgr = static_cast<PhysicsManagerService*>(app->getService("physics_mgr"));
        if (!mgr) {
            mgr = new PhysicsManagerService("PhysicsManagerService", "//meworla/physicsmanagerservice");
            mgr->setup(app);           // if you have setup()
            app->registerService("PhysicsManager", mgr);
        }

        // register null backend
        mgr->registerBackend("none", [](Application* app)->std::unique_ptr<PhysicsService> {
            return std::make_unique<NullPhysicsService>("PhysicsService", "//meworla/service");			
            });

        // register a default for all types
        PhysicsBodyDesc rigidbody;
        rigidbody.kind = "Meworla/RheaPhysics:RigidBody";
        mgr->registerBodyForType(Item::staticType(), rigidbody);

        // opt-out: no creator - move to rheaclient/fabrics
        PhysicsBodyDesc none;
        none.kind = ""; // means disabled
        mgr->registerBodyForType(Item2D::staticType(), none);
	}
}