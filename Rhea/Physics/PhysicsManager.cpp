#include "PhysicsManager.h"
#include "Bullet/BtPhysicsService.h"

namespace Rhea
{
	PhysicsService* PhysicsManagerService::getPhysicsService(Types type)
	{
		PhysicsService* s;
		switch (type) {
		case Types::NONE:
			s = new PhysicsService("PhysicsService", "/internal/physics");
			s->setup(m_app);
			return s;
		case Types::WORLD:
			s =  new BulletPhysicsService("PhysicsService", "/internal/physics");
			s->setup(m_app);
			return s;
		}
		return nullptr;
	}

	void PhysicsManagerService::onInit()
	{
	}

	// factory function
	Unit* PhysicsManagerService::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new PhysicsManagerService(name, type);
		return ret;
	}
}