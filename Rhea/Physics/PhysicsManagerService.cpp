#include <Rhea/Physics/PhysicsManagerService.h>
#include <Rhea/Physics/PhysicsService.h>
#include <Rhea/Physics/PhysicsBody.h>
#include <Rhea/Core/Item.h>

namespace Rhea
{
	PhysicsManagerService::PhysicsManagerService(std::string name, std::string type_name)
		: Service(std::move(name), std::move(type_name))
	{
	}

	void PhysicsManagerService::registerBackend(const std::string& id, BackendCreator creator)
	{
		m_backend_creators[id] = std::move(creator);
	}

	void PhysicsManagerService::registerBodyForType(SimpleParser::Type* typedesc, PhysicsBodyDesc desc)
	{
		if (!typedesc) return;
		m_type_to_body[typedesc] = std::move(desc);
	}

	const PhysicsBodyDesc* PhysicsManagerService::getBodyDescFor(Item* item) const
	{
		if (!item) return nullptr;
		SimpleParser::Type* t = item->getType();
		while (t) {
			auto it = m_type_to_body.find(t);
			if (it != m_type_to_body.end())
				return &it->second;
			t = t->getParent();
		}
		return nullptr;
	}

	void PhysicsManagerService::registerPhysicsComponent(const std::string& name, ComponentCreator creator)
	{
		m_component_creators[name] = std::move(creator);

		// if a service already exists, forward register immediately
		if (m_phy_service) {
			m_phy_service->registerPhysicsComponent(name, m_component_creators[name]);
		}
	}

	PhysicsService* PhysicsManagerService::createPhysicsService(const std::string& backend_id)
	{
		auto it = m_backend_creators.find(backend_id);
		if (it == m_backend_creators.end()) {
			it = m_backend_creators.find("none");
			if (it == m_backend_creators.end()) return nullptr;
		}

		m_phy_service = it->second(m_app);         // m_app from Service base
		m_phy_service->setup(m_app);

		// forward all already registered component creators
		for (auto& kv : m_component_creators) {
			m_phy_service->registerPhysicsComponent(kv.first, kv.second);
		}
		
		for (auto& h : m_backend_hooks) h(m_phy_service.get());

		return m_phy_service.get();
	}

	void PhysicsManagerService::onInit()
	{
		// optional: choose default backend if desired, or do nothing
		// createPhysicsService("none");
	}

	// note: currently physics_service is stored directly in application
	bool PhysicsManagerService::onPrePhysics(float delta)
	{
		if (m_phy_service)
			return m_phy_service->onPrePhysics(delta);
		return false;
	}

	void PhysicsManagerService::onPhysicsStep(float frame_slice)
	{
		if (m_phy_service)
			m_phy_service->onPhysicsStep(frame_slice);
	}

	bool PhysicsManagerService::onFrame(float delta)
	{
		if (m_phy_service)
			return m_phy_service->onFrame(delta);
		return false;
	}

	bool PhysicsManagerService::onFrameFinal(float delta)
	{
		if (m_phy_service)
			return m_phy_service->onFrameFinal(delta);
		return false;
	}
}