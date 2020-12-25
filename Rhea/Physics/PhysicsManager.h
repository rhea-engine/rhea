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
		PhysicsManagerService(std::string name, std::string type_name) :
			Service("PhysicsManagerService", type_name)
		{}

		PhysicsService* getPhysicsService(Types type);

		// Service functions
		virtual void onInit();

		// factory function
		static Unit* createType(std::string name, std::string type);
	};
}

#endif // !RHEA_PHYSICS_MGR_H
