#ifndef RHEA_PHYSICS_MGR_H
#define RHEA_PHYSICS_MGR_H

#include "../Service.h"
#include "PhysicsService.h"

namespace Rhea
{

	class PhysicsManagerService : public Service
	{
	public:
		enum class Types
		{
			NONE,
			WORLD
		};
		PhysicsManagerService(Application* app) :
			Service(app, "PhysicsManagerService")
		{}

		PhysicsService* getPhysicsService(Types type);

		// Service functions
		virtual void onInit();
	};
}

#endif // !RHEA_PHYSICS_MGR_H
