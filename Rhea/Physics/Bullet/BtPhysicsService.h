#ifndef RHEA_BULLET_PHYSICS_SERVICE_H
#define RHEA_BULLET_PHYSICS_SERVICE_H

#include "../PhysicsService.h"

class btDynamicsWorld;

namespace Rhea
{
	class Controller;
	class Scene;

	class BulletPhysicsService : public PhysicsService
	{
	public:
		BulletPhysicsService(Application* app);

		// process physics with delta seconds (usually 1/60)
		void step(float delta);

		// get the physics world
		btDynamicsWorld* getWorld() { return m_world; }

		// set scene and initialize objects in it
		void setScene(Scene* scene);

		// Service functions
		void onInit();

	protected:
		btDynamicsWorld* m_world;
		Scene* m_scene;
		bool m_debug;
	};
}

#endif // !RHEA_BULLET_PHYSICS_SERVICE_H
