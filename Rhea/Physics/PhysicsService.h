#ifndef RHEA_PHYSICS_SERVICE_H
#define RHEA_PHYSICS_SERVICE_H

#include "../Service.h"
#include <Rhea/Core/Scene.h>
#include <Math/Vector3.h>
#include "Body.h"

namespace Rhea
{
	class Controller;
	class Item;

	class PhysicsService : public Service
	{
		MECLASS(PhysicsService, Service)
	public:
		PhysicsService::PhysicsService(std::string name, std::string type_name) :
			m_item(nullptr),			
			Service(name, type_name)
		{
		}

		// process physics with delta seconds (usually 1/60)
		void onPhysicsStep(float delta) override;

		// set scene and initialize objects in it
		virtual void setScene(Scene* scene);		

		// initalize one object
		virtual void initializeItem(Item* item);

		// remove one object
		virtual void removeItem(Item* item);

		// Service functions
		void onInit();	

		// toggle debug draw
		virtual void toggleDebug() {}

		// ray trace
		virtual Unit* ray(Math::Vector3 origin, Math::Vector3 direction) { return nullptr; };

		// physics component registry
		using CreatorFunc = std::function<PhysicsBody*(Item*)>;
		void registerPhysicsComponent(const std::string& name, CreatorFunc func);
	protected:
		Item* m_item;		
		std::unordered_map<std::string, CreatorFunc> m_physics_component_registry;
	};
}

#endif // !RHEA_PHYSICS_SERVICE_H
