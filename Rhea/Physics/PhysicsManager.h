#ifndef RHEA_PHYSICS_MGR_H
#define RHEA_PHYSICS_MGR_H

#include "../Service.h"
#include "PhysicsService.h"

namespace Rhea
{

	class PhysicsManagerService : public Service
	{
		MECLASS(PhysicsManagerService, Service)
	public:
		enum class Types
		{
			NONE,
			WORLD
		};
		PhysicsManagerService(std::string name, std::string type_name) :
			m_phy_services(nullptr),
			Service("PhysicsManagerService", type_name)
		{}

		PhysicsService* createPhysicsService(Types type);
		PhysicsService* getPhysicsService() { return m_phy_services; }

		// Service functions
		virtual void onInit();

		// physics component registry
		using CreatorFunc = std::function<PhysicsBody* (Item*)>;
		void registerPhysicsComponent(const std::string& name, CreatorFunc func);

	protected:
		PhysicsService* m_phy_services;
		std::unordered_map<std::string, CreatorFunc> m_physics_component_registry;
	};
}

#endif // !RHEA_PHYSICS_MGR_H
