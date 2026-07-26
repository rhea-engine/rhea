/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_BULLET_PHYSICS_SERVICE_H
#define RHEA_BULLET_PHYSICS_SERVICE_H

#include <Rhea/Physics/PhysicsService.h>
#include <Rhea/Physics/PhysicsBodyDescriptor.h>
#include <Rhea/Physics/IPhysicsDebugDraw.h>

class btDynamicsWorld;
class btIDebugDraw;
class btRigidBody;

namespace rhea
{
	class Controller;
	class Scene;
	class PhysicsDataComponent;
}

namespace rhea::physics::bullet
{
	class BulletPhysicsService : public rhea::PhysicsService
	{
		RECLASS(BulletPhysicsService, rhea::PhysicsService, "//meworla/bulletphysicsservice")
	public:
		BulletPhysicsService(std::string name, std::string type_name);		

		// process physics with delta seconds (usually 1/60)
		void onPhysicsStep(float delta);

		// get the physics world
		btDynamicsWorld* getWorld() { return m_world; }

		// set scene and initialize objects in it
		void activateScene(rhea::Scene* scene);
		// deactivate scene and bulk deregister items
		void deactivateScene(rhea::Scene* scene);

		// initalize one object
		void initializeItem(rhea::Item* item);

		// remove one object
		void removeItem(rhea::Item* item);

		// Service functions
		void onInit();

		// ray trace
		Unit* ray(math::Vector3 origin, math::Vector3 direction);
		bool sphereCast(const math::Vector3& from, const math::Vector3& to, float radius, PhysicsQueryHit& hit, const Unit* ignoredUnit = nullptr);

		std::unique_ptr<rhea::PhysicsBody> createBody(const rhea::PhysicsDataComponent* pdc, rhea::Item* item);

		using BulletDrawerFactory = std::function<rhea::IPhysicsDebugDraw* ()>;
		void setDrawerFactory(BulletDrawerFactory f) { m_drawer_factory = f; }
		rhea::IPhysicsDebugDraw* getDebugDrawer() override { if (!m_debug_drawer && m_drawer_factory) m_debug_drawer = m_drawer_factory(); return m_debug_drawer; }
		
		bool onItemAdd(rhea::Event* ev, rhea::Unit* unit);
		bool onItemRemove(rhea::Event* ev, rhea::Unit* unit);

		void dumpStats() const override;

	protected:
		struct ScenePhysicsState
		{
			btRigidBody* ground = nullptr;
		};
		btDynamicsWorld* m_world;
		BulletDrawerFactory m_drawer_factory;
		rhea::IPhysicsDebugDraw* m_debug_drawer;
		std::unordered_map<Scene*, ScenePhysicsState> m_scene_states;
	};
}

#endif // !RHEA_BULLET_PHYSICS_SERVICE_H
