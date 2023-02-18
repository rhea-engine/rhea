#ifndef RHEA_BULLET_PHYSICS_SERVICE_H
#define RHEA_BULLET_PHYSICS_SERVICE_H

#include "../PhysicsService.h"

class btDynamicsWorld;

namespace Rhea
{
	class Controller;
	class Scene;
	class BulletDebugDraw;

	class BulletPhysicsService : public PhysicsService
	{
	public:
		BulletPhysicsService(std::string name, std::string type_name);

		// process physics with delta seconds (usually 1/60)
		void step(float delta);

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

	protected:
		btDynamicsWorld* m_world;
		Scene* m_scene;
	};
}

#endif // !RHEA_BULLET_PHYSICS_SERVICE_H
