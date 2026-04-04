#ifndef RHEA_PHYSICS_SERVICE_H
#define RHEA_PHYSICS_SERVICE_H

#include <Rhea/Service.h>
#include <Rhea/Core/Scene.h>
#include <Math/Vector3.h>
#include <Rhea/Physics/PhysicsService.h>
#include "NullBody.h"

namespace rhea
{
	class Controller;
	class Item;
}

namespace rhea::physics
{
	class NullPhysicsService : public rhea::PhysicsService
	{
		MECLASS(NullPhysicsService, rhea::PhysicsService)
	public:
		NullPhysicsService(std::string name, std::string type_name) :	
			rhea::PhysicsService(name, type_name)
		{
		}

		// process physics with delta seconds (usually 1/60)
		void onPhysicsStep(float delta) override;

		// set scene and initialize objects in it
		void setScene(rhea::Scene* scene) override;		

		// initalize one object
		void initializeItem(rhea::Item* item) override;

		// remove one object
		void removeItem(rhea::Item* item) override;

		// Service functions
		void onInit();	

		// toggle debug draw
		void toggleDebug() override {}

		// ray trace
		Unit* ray(Math::Vector3 origin, Math::Vector3 direction) override { return nullptr; };
	};
}

#endif // !RHEA_PHYSICS_SERVICE_H
