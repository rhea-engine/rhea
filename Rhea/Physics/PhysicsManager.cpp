#include "PhysicsManager.h"
#include "Bullet/BtPhysicsService.h"

namespace Rhea
{
	PhysicsService* PhysicsManagerService::createPhysicsService(Types type)
	{
		switch (type) {
		case Types::NONE:
			m_phy_services = new PhysicsService("PhysicsService", "//meworla/service");
			m_phy_services->setup(m_app);
			break;
		case Types::WORLD:
			m_phy_services =  new BulletPhysicsService("PhysicsService", "//meworla/service");
			m_phy_services->setup(m_app);
			break;
		}
		return m_phy_services;
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