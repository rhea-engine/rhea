#include <Rhea/Terrain/TerrainFabrics.h>

#include <Rhea/Terrain/Terrain.h>
#include <Rhea/Fabrics/Media/ImageResource.h>
#include <Psio/Log.h>
#include <Rhea/Physics/PhysicsService.h>
#include <Rhea/Terrain/BulletHeightMap.h>

namespace Rhea
{
	void TerrainFabrics::Register(Application* app)
	{
		SimpleParser::Type::registerType<ImageResource>("//meworla/heightmap");
		SimpleParser::Type::registerType<Terrain>("//meworla/terrain");

		//register physics component
		PhysicsService* phy_mgr = doCast<PhysicsService>(app->getService("physics"));
		if (phy_mgr) {
			phy_mgr->registerPhysicsComponent("//meworla/terrain", [](Item* item) -> PhysicsBody* {
				return new BulletHeightMap(nullptr, item);
				});
		}
		else {
			LOG(LOG_ERROR) << "No physics manager found.";
		}
	}
}