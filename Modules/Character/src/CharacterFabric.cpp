#include <Rhea/Character/CharacterFabric.h>

#include <Rhea/Character/KinematicCharacter.h>
#include <Psio/Log.h>
#include <Rhea/Physics/PhysicsManager.h>
#include <Rhea/Physics/Bullet/BtCharacter.h>
#include <Rhea/Physics/Bullet/BtPhysicsService.h>


namespace Rhea
{
	void CharacterFabric::Register(Application* app)
	{
		SimpleParser::Type::registerType<KinematicCharacter>("//meworla/character");

		//register physics component
		PhysicsManagerService* phy_mgr = doCast<PhysicsManagerService>(app->getService("physics_mgr"));
		if (phy_mgr) {
			phy_mgr->registerPhysicsComponent("KinematicCharacter", [app](Item* item) -> PhysicsBody* {
				// get world from service
				BulletPhysicsService* bt_service = static_cast<BulletPhysicsService*>(app->getService("physics"));
				if (!bt_service) {
					LOG(LOG_ERROR) << "KinematicCharacter:registerPhysicsComponent: Physics Service not found";
					return nullptr;
				}
				btDynamicsWorld* world = bt_service->getWorld();
				return new BulletPhysicsCharacter(world, item);
				});
		}
		else {
			LOG(LOG_ERROR) << "No physics manager found.";
		}
	}
}