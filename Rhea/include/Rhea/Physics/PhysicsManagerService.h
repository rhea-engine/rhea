#ifndef RHEA_PHYSICS_MGR_H
#define RHEA_PHYSICS_MGR_H

#include <Rhea/Service.h>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include "PhysicsBody.h"
#include "PhysicsBodyDescriptor.h"

namespace Rhea
{
	class Application;
	class Item;
	class PhysicsService;

	class PhysicsManagerService : public Service
	{
		MECLASS(PhysicsManagerService, Service)
	public:
		using ComponentCreator = std::function<std::unique_ptr<PhysicsBody>(PhysicsService*, Item*)>;
		using BackendHook = std::function<void(PhysicsService*)>;

		// Prefer unique_ptr to avoid manual delete
		using BackendCreator = std::function<std::unique_ptr<PhysicsService>(Application* app)>;

		PhysicsManagerService(std::string name, std::string type_name);

		// Backends call this during module/fabric register
		void registerBackend(const std::string& id, BackendCreator creator);

		void registerBodyForType(SimpleParser::Type* typedesc, PhysicsBodyDesc desc);
		const PhysicsBodyDesc* getBodyDescFor(Item* item) const;

		// Components register their body creators here
		void registerPhysicsComponent(const std::string& name, ComponentCreator creator);

		// Choose backend at runtime ("none", "world", "physx", ...)
		PhysicsService* createPhysicsService(const std::string& backend_id);

		PhysicsService* getPhysicsService() const { return m_phy_service.get(); }

		void addBackendHook(BackendHook hook) {
			m_backend_hooks.push_back(std::move(hook));
			if (m_phy_service) m_backend_hooks.back()(m_phy_service.get());
		}

		// Service functions
		void onInit() override;
		bool onPrePhysics(float delta) override;
		void onPhysicsStep(float frame_slice) override;
		bool onFrame(float delta) override;
		bool onFrameFinal(float delta) override;

	private:
		std::unique_ptr<PhysicsService> m_phy_service;

		std::unordered_map<std::string, BackendCreator> m_backend_creators;
		std::unordered_map<std::string, ComponentCreator> m_component_creators;
		std::vector<BackendHook> m_backend_hooks;

		std::unordered_map<SimpleParser::Type*, PhysicsBodyDesc> m_type_to_body;
	};		
}

#endif // !RHEA_PHYSICS_MGR_H
