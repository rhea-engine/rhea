#include <Rhea/Character/CharacterFabric.h>

#include <Rhea/Character/KinematicCharacter.h>
#include <Psio/Log.h>
#include <Rhea/Physics/PhysicsManagerService.h>

namespace rhea
{
	void CharacterFabric::Register(Application* app)
	{
		simpleparser::Type::registerType<KinematicCharacter>("//meworla/character");

		//register physics component
		PhysicsManagerService* phy_mgr = doCast<PhysicsManagerService>(app->getService("physics_mgr"));
		if (!phy_mgr) {
			LOG(LOG_ERROR) << "No physics manager found.";
			return;
		}
		simpleparser::Type* t = KinematicCharacter::staticType();		

		rhea::PhysicsBodyDesc desc;
		desc.kind = "Meworla/RheaPhysics:Character";
		desc.kinematic = true;
		desc.mass = 70.0f; // optional
		phy_mgr->registerBodyForType(t, std::move(desc));
	}
}