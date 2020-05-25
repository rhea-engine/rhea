#include "PhysicsManager.h"
#include "Bullet/BtPhysicsService.h"

namespace Rhea
{
	PhysicsService* PhysicsManagerService::getPhysicsService(Types type)
	{
		switch (type) {
		case Types::NONE:
			return new PhysicsService(m_app);
		case Types::WORLD:
			return new BulletPhysicsService(m_app);
		}
		return nullptr;
	}

	void PhysicsManagerService::onInit()
	{
	}
}