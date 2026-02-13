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
		// set/copy physics components to physics service
		for (const auto& kv : m_physics_component_registry) {
			const std::string& name = kv.first;
			const CreatorFunc& creator = kv.second;
			m_phy_services->registerPhysicsComponent(name, creator);
		}
		return m_phy_services;
	}

	void PhysicsManagerService::onInit()
	{
	}

	void PhysicsManagerService::registerPhysicsComponent(const std::string& name, CreatorFunc func)
	{
		m_physics_component_registry[name] = func;
	}
}