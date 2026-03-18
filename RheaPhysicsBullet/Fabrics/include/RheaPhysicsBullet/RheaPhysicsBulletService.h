#ifndef RHEA_BULLET_PHYSICS_SERVICE_H
#define RHEA_BULLET_PHYSICS_SERVICE_H

#include <Rhea/Physics/PhysicsService.h>
#include <Rhea/Physics/PhysicsBodyDescriptor.h>
#include <Rhea/Physics/IPhysicsDebugDraw.h>

class btDynamicsWorld;
class btIDebugDraw;

namespace Rhea
{
	class Controller;
	class Scene;

	class BulletPhysicsService : public PhysicsService
	{
		MECLASS(BulletPhysicsService, PhysicsService)
	public:
		BulletPhysicsService(std::string name, std::string type_name);		

		// process physics with delta seconds (usually 1/60)
		void onPhysicsStep(float delta);

		// get the physics world
		btDynamicsWorld* getWorld() { return m_world; }

		// set scene and initialize objects in it
		void setScene(Scene* scene);

		// initalize one object
		void initializeItem(Item* item);

		// remove one object
		void removeItem(Item* item);

		// Service functions
		void onInit();

		// ray trace
		Unit* ray(Math::Vector3 origin, Math::Vector3 direction);

		std::unique_ptr<PhysicsBody> createBody(const PhysicsBodyDesc& desc, Item* item);

		void setDebugDrawer(btIDebugDraw* drawer);

		using BulletDrawerFactory = std::function<IPhysicsDebugDraw* ()>;
		void setDrawerFactory(BulletDrawerFactory f) { m_drawer_factory = f; }
		IPhysicsDebugDraw* debugDraw() override { if (!m_debug_drawer && m_drawer_factory) m_debug_drawer = m_drawer_factory(); return m_debug_drawer; }

	protected:
		btDynamicsWorld* m_world;
		Scene* m_scene;
		BulletDrawerFactory m_drawer_factory;
		IPhysicsDebugDraw* m_debug_drawer;
	};
}

#endif // !RHEA_BULLET_PHYSICS_SERVICE_H
