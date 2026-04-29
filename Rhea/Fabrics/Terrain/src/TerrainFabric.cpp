/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Terrain/TerrainFabric.h>

#include <Rhea/Terrain/Terrain.h>
#include <Rhea/Resources/ImageResource.h>
#include <Psio/Log.h>
#include <Rhea/Physics/PhysicsManagerService.h>

namespace rhea
{
	void TerrainFabric::Register(Application* app)
	{
		simpleparser::Type::registerType<ImageResource>("//meworla/heightmap");
		simpleparser::Type::registerType<Terrain>("//meworla/terrain");

		//register physics component
		PhysicsManagerService* phy_mgr = doCast<PhysicsManagerService>(app->getService("physics_mgr"));
		if (!phy_mgr) {
			LOG(LOG_ERROR) << "No physics manager found.";
			return;
		}
				
		simpleparser::Type* t = Terrain::staticType();

		rhea::PhysicsBodyDesc desc;
		desc.kind = "Meworla/RheaPhysics:HeightMap";
		desc.mass = 0.0f;     // static
		phy_mgr->registerBodyForType(t, desc);
	}
}