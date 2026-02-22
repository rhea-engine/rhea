#include <Rhea/Terrain/TerrainFabric.h>

#include <Rhea/Terrain/Terrain.h>
#include <Rhea/Fabrics/Media/ImageResource.h>
#include <Psio/Log.h>
#include <Rhea/Physics/PhysicsManagerService.h>

namespace Rhea
{
	void TerrainFabric::Register(Application* app)
	{
		SimpleParser::Type::registerType<ImageResource>("//meworla/heightmap");
		SimpleParser::Type::registerType<Terrain>("//meworla/terrain");

		//register physics component
		PhysicsManagerService* phy_mgr = doCast<PhysicsManagerService>(app->getService("physics_mgr"));
		if (!phy_mgr) {
			LOG(LOG_ERROR) << "No physics manager found.";
			return;
		}

		/*
			phy_mgr->registerPhysicsComponent("Terrain", [](Item* item) ->  std::unique_ptr<PhysicsBody> {
				return std::make_unique<BulletHeightMap>(nullptr, item);
				});
				*/
					
		SimpleParser::Type* t = Terrain::staticType();

		Rhea::PhysicsBodyDesc desc;
		desc.kind = "Meworla/RheaPhysics:HeightMap";
		desc.mass = 0.0f;     // static
		phy_mgr->registerBodyForType(t, desc);
	}
}