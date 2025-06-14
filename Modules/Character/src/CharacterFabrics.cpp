#include <Rhea/Character/CharacterFabrics.h>

#include <Rhea/Character/KinematicCharacter.h>
#include <Psio/Log.h>
#include <Rhea/Physics/PhysicsService.h>
#include <Rhea/Physics/Bullet/BtCharacter.h>
#include <Rhea/Physics/Bullet/BtPhysicsService.h>


namespace Rhea
{
	void CharacterFabrics::Register(Application* app)
	{
		SimpleParser::Type::registerType<KinematicCharacter>("//meworla/character");

		//register physics component
		PhysicsService* phy_mgr = doCast<PhysicsService>(app->getService("physics"));
		if (phy_mgr) {
			phy_mgr->registerPhysicsComponent("//meworla/character", [phy_mgr](Item* item) -> PhysicsBody* {
				// get world from service
				btDynamicsWorld* world = static_cast<BulletPhysicsService*>(phy_mgr)->getWorld();
				return new BulletPhysicsCharacter(world, item);
				});
		}
		else {
			LOG(LOG_ERROR) << "No physics manager found.";
		}
	}
}