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

}

namespace rhea::physics::bullet
{
	class BulletPhysicsService : public Rhea::PhysicsService
	{
		MECLASS(BulletPhysicsService, Rhea::PhysicsService)
	public:
		BulletPhysicsService(std::string name, std::string type_name);		

		// process physics with delta seconds (usually 1/60)
		void onPhysicsStep(float delta);

		// get the physics world
		btDynamicsWorld* getWorld() { return m_world; }

		// set scene and initialize objects in it
		void setScene(Rhea::Scene* scene);

		// initalize one object
		void initializeItem(Rhea::Item* item);

		// remove one object
		void removeItem(Rhea::Item* item);

		// Service functions
		void onInit();

		// ray trace
		Unit* ray(Math::Vector3 origin, Math::Vector3 direction);

		std::unique_ptr<Rhea::PhysicsBody> createBody(const Rhea::PhysicsBodyDesc& desc, Rhea::Item* item);

		void setDebugDrawer(btIDebugDraw* drawer);

		using BulletDrawerFactory = std::function<Rhea::IPhysicsDebugDraw* ()>;
		void setDrawerFactory(BulletDrawerFactory f) { m_drawer_factory = f; }
		Rhea::IPhysicsDebugDraw* debugDraw() override { if (!m_debug_drawer && m_drawer_factory) m_debug_drawer = m_drawer_factory(); return m_debug_drawer; }

	protected:
		btDynamicsWorld* m_world;
		Rhea::Scene* m_scene;
		BulletDrawerFactory m_drawer_factory;
		Rhea::IPhysicsDebugDraw* m_debug_drawer;
	};
}

#endif // !RHEA_BULLET_PHYSICS_SERVICE_H
