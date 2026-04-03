#ifndef RHEA_PHYSICS_SERVICE_H
#define RHEA_PHYSICS_SERVICE_H

#include <Rhea/Service.h>
#include <Rhea/Core/Scene.h>
#include <Math/Vector3.h>
#include <Rhea/Physics/PhysicsService.h>
#include "NullBody.h"

namespace Rhea
{
	class Controller;
	class Item;

	class NullPhysicsService : public PhysicsService
	{
		MECLASS(NullPhysicsService, PhysicsService)
	public:
		NullPhysicsService(std::string name, std::string type_name) :	
			PhysicsService(name, type_name)
		{
		}

		// process physics with delta seconds (usually 1/60)
		void onPhysicsStep(float delta) override;

		// set scene and initialize objects in it
		void setScene(Scene* scene) override;		

		// initalize one object
		void initializeItem(Item* item) override;

		// remove one object
		void removeItem(Item* item) override;

		// Service functions
		void onInit();	

		// toggle debug draw
		void toggleDebug() override {}

		// ray trace
		Unit* ray(Math::Vector3 origin, Math::Vector3 direction) override { return nullptr; };
	};
}

#endif // !RHEA_PHYSICS_SERVICE_H
