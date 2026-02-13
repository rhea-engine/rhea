#include <Rhea/Terrain/TerrainFabrics.h>

#include <Rhea/Terrain/Terrain.h>
#include <Rhea/Fabrics/Media/ImageResource.h>
#include <Psio/Log.h>
#include <Rhea/Physics/PhysicsManager.h>
#include <Rhea/Terrain/BulletHeightMap.h>

namespace Rhea
{
	void TerrainFabrics::Register(Application* app)
	{
		SimpleParser::Type::registerType<ImageResource>("//meworla/heightmap");
		SimpleParser::Type::registerType<Terrain>("//meworla/terrain");

		//register physics component
		PhysicsManagerService* phy_mgr = doCast<PhysicsManagerService>(app->getService("physics_mgr"));
		if (phy_mgr) {
			phy_mgr->registerPhysicsComponent("Terrain", [](Item* item) -> PhysicsBody* {
				return new BulletHeightMap(nullptr, item);
				});
		}
		else {
			LOG(LOG_ERROR) << "No physics manager found.";
		}
	}
}