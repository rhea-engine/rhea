/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <RheaPhysicsBullet/RheaPhysicsBulletService.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Core/Scene.h>
#include "BtBody.h"
#include "BtGhost.h"
#include "BtCharacter.h"

#include <bullet/src/btBulletDynamicsCommon.h>
#include <bullet/src/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <Psio/Log.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>

#include <assert.h>
#include <RheaPhysics/PhysicsComponent.h>
#include <Rhea/Core/Camera.h>

#include "BulletHeightMap.h"
#include <Rhea/Physics/PhysicsManagerService.h>
#include <Rhea/Physics/PhysicsEvent.h>
#include <Rhea/Physics/CollisionEvent.h>

namespace rhea::physics::bullet
{
	BulletPhysicsService::BulletPhysicsService(std::string name, std::string type_name) :
		m_world(nullptr),
		m_scene(nullptr),
		m_debug_drawer(nullptr),
		rhea::PhysicsService("BulletPhysicsService", type_name)
	{
	}

	void BulletPhysicsService::onPhysicsStep(float delta)
	{
		if (!m_world || !m_scene) {		
			assert(false);
			return;
		}
		std::unique_ptr<rhea::PhysicsEvent> ev = std::make_unique<rhea::PhysicsEvent>("physics_pre", delta);
		sendEvent(std::string("pre_physics"), ev.get());

		std::vector<rhea::Item*> items = m_scene->getItems();

		/*
		for (auto it = items.begin(); it != items.end(); ++it) {
			Item* item = *it;
			BulletPhysicsBody* body = static_cast<BulletPhysicsBody*>(item->getPhysicsBody());
			if (body)
				body->doPhysics(delta, m_terrain);
		}
		*/

		m_world->stepSimulation(delta, 1);

		// Collision Detection durchführen
		int numManifolds = m_world->getDispatcher()->getNumManifolds();
		for (int i = 0; i < numManifolds; i++)
		{
			btPersistentManifold* contactManifold = m_world->getDispatcher()->getManifoldByIndexInternal(i);
			const btCollisionObject* objA = contactManifold->getBody0();
			const btCollisionObject* objB = contactManifold->getBody1();

			rhea::Item* itemA = doCast<rhea::Item>((simpleparser::Variant*)objA->getUserPointer());
			rhea::Item* itemB = doCast<rhea::Item>((simpleparser::Variant*)objB->getUserPointer());

			if (!itemA || !itemB) continue;

			int numContacts = contactManifold->getNumContacts();
			for (int j = 0; j < numContacts; j++)
			{
				btManifoldPoint& pt = contactManifold->getContactPoint(j);
				if (pt.getDistance() < 0.f)
				{
					const btVector3& ptA = pt.getPositionWorldOnA();
					const btVector3& normalOnB = pt.m_normalWorldOnB;

					math::Vector3 collisionPoint(ptA.getX(), ptA.getY(), ptA.getZ());
					math::Vector3 collisionNormal(normalOnB.getX(), normalOnB.getY(), normalOnB.getZ());

					// Event an beide Items senden
					auto evA = std::make_unique<rhea::CollisionEvent>(itemA, itemB, collisionPoint, collisionNormal);
					itemA->sendEvent("collision", evA.get());

					auto evB = std::make_unique<rhea::CollisionEvent>(itemB, itemA, collisionPoint, -collisionNormal);
					itemB->sendEvent("collision", evB.get());
				}
			}
		}

		// HINZUGEFÜGT: Ghost Object Kollisionen für Kinematic Bodies
		for (int j = m_world->getNumCollisionObjects() - 1; j >= 0; j--)
		{
			btCollisionObject* obj = m_world->getCollisionObjectArray()[j];
			
			// Prüfen ob es ein Ghost Object ist
			if (obj->getInternalType() == btCollisionObject::CO_GHOST_OBJECT)
			{
				btGhostObject* ghost = static_cast<btGhostObject*>(obj);
				rhea::Item* itemA = doCast<rhea::Item>((simpleparser::Variant*)ghost->getUserPointer());
				
				if (itemA) {
					int numOverlaps = ghost->getNumOverlappingObjects();
					for (int k = 0; k < numOverlaps; k++)
					{
						btCollisionObject* overlappingObj = ghost->getOverlappingObject(k);
						rhea::Item* itemB = doCast<rhea::Item>((simpleparser::Variant*)overlappingObj->getUserPointer());
						
						if (itemB) {
							// Collision Event senden (ohne genaue Kontaktpunkte)
							auto evA = std::make_unique<rhea::CollisionEvent>(itemA, itemB, math::Vector3(0,0,0), math::Vector3(0,1,0));
							itemA->sendEvent("collision", evA.get());
						}
					}
				}
			}
		}

		for (int j = m_world->getNumCollisionObjects() - 1; j >= 0; j--)
		{
			btCollisionObject* obj = m_world->getCollisionObjectArray()[j];

			if (obj->getInternalType() == btCollisionObject::CO_GHOST_OBJECT)
				continue;

			rhea::Item* item = doCast<rhea::Item>((simpleparser::Variant*)obj->getUserPointer());

			// TODO: optimize this by keeping track of dynamic objects in the world and only updating those
			// in the future we may also want to update kinematic objects here, but for now we only update dynamic objects, which are the most common ones to move around
			// 
			// Pseudo code for optimization:
			// pyhsics service keeps a list of dynamic objects in the world, which is updated when objects are added or removed from the world
			// service->registerPhysicsComponent(name, func) adds to the registry and also adds to the list of dynamic objects if the component is dynamic
			// for (auto* obj : dynamic_objects) {
			//		auto* body = obj->getComponent<PhysicsComponent>() ? obj->getComponent<PhysicsComponent>()->getBody() : nullptr;
			//		if (body && body->isDynamic()) {
			//			...
			//		}
			// }
			// 
			
			// update position for dynamic objects from physics engine
			if (!item) continue;			

			auto* body = item->getComponent<PhysicsComponent>() ?
				item->getComponent<PhysicsComponent>()->getBody() :
				nullptr;

			if (body && body->isDynamic()) {

				btRigidBody* body = btRigidBody::upcast(obj);
				btTransform trans;
				if (body && body->getMotionState())
				{
					body->getMotionState()->getWorldTransform(trans);
				}
				else
				{
					trans = obj->getWorldTransform();
				}
				rhea::Position position = *item->getTransform()->get_position();

				position.setPosition(math::Vector3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ())));
				btQuaternion rot = trans.getRotation();
				position.setOrientation(math::Quaternion(rot.getW(), rot.getX(), rot.getY(), rot.getZ()));

				item->setPosition(position);				
			}
		}
	}
	
	void BulletPhysicsService::setScene(rhea::Scene* scene)
	{
		if (!scene) {
			LOG(LOG_ERROR) << "BulletPhysicsService::setScene: no scene";
			return;
		}

		btAlignedObjectArray<btCollisionShape*> collisionShapes;
		bool defaultGround = scene->getValue("ground");

		if (defaultGround) {
			// add default ground (500x500x50, 1 under ground)
			btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(500.), btScalar(50.), btScalar(500.)));

			collisionShapes.push_back(groundShape);

			btTransform groundTransform;
			groundTransform.setIdentity();
			groundTransform.setOrigin(btVector3(0, -51, 0));

			btScalar mass(0.);

			//rigidbody is dynamic if and only if mass is non zero, otherwise static
			bool isDynamic = (mass != 0.f);

			btVector3 localInertia(0, 0, 0);
			if (isDynamic)
				groundShape->calculateLocalInertia(mass, localInertia);

			//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
			btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
			m_ground = new btRigidBody(rbInfo);

			//add the body to the dynamics world
			m_world->addRigidBody(m_ground);
		}

		// get items of scene
		m_scene = scene;
		std::vector<rhea::Item*> items = m_scene->getItems();

		for (auto it = items.begin(); it != items.end(); ++it) {
			rhea::Item* item = *it;
			if (item->getComponent<PhysicsComponent>())
				continue;

			initializeItem(item);
		}

		scene->registerEventHandler(
			"scene_item_added", this,
			std::bind(&BulletPhysicsService::onItemAdd, this, std::placeholders::_1, std::placeholders::_2)
		);
		scene->registerEventHandler(
			"scene_item_removed", this,
			std::bind(&BulletPhysicsService::onItemRemove, this, std::placeholders::_1, std::placeholders::_2)
		);
	}

	void BulletPhysicsService::clearScene(rhea::Scene* scene)
	{
		if (!scene) {
			LOG(LOG_ERROR) << "BulletPhysicsService::clearScene: no scene";
			return;
		}
		if (m_ground)
		{
			m_world->removeRigidBody(m_ground);

			delete m_ground->getMotionState();
			delete m_ground;

			m_ground = nullptr;
		}

		std::vector<rhea::Item*> items = m_scene->getItems();

		for (auto it = items.begin(); it != items.end(); ++it) {
			rhea::Item* item = *it;
			auto* pc = item->getComponent<PhysicsComponent>();
			if (!pc)
				continue;
			pc->deleteBody();
		}

		scene->unregisterEventHandler("scene_item_added", this);
		scene->unregisterEventHandler("scene_item_removed", this);
	}

	bool BulletPhysicsService::onItemAdd(rhea::Event* ev, rhea::Unit* unit)
	{
		rhea::Item* item = doCast<rhea::Item>(ev->getTarget());
		if (item)
			initializeItem(item);

		return false;
	}

	bool BulletPhysicsService::onItemRemove(rhea::Event* ev, rhea::Unit* unit)
	{
		rhea::Item* item = doCast<rhea::Item>(ev->getTarget());
		if (!item) return false;

		if (auto* pc = item->getComponent<PhysicsComponent>()) {			
			pc->deleteBody();
			item->deleteComponent<PhysicsComponent>();
		}
		return false;
	}

	void BulletPhysicsService::initializeItem(rhea::Item* item)
	{
		auto* mgr = doCast<rhea::PhysicsManagerService>(m_app->getService("physics_mgr"));		
		const auto* desc = mgr->getBodyDescFor(item);
		if (!desc || desc->kind.empty()) return;

		auto body = createBody(*desc, item); // bullet decides by desc.kind
		auto* bt = static_cast<BulletPhysicsBody*>(body.get()); // safe in this backend
		if (bt && bt->init()) {
			auto* pc = item->addComponentUnique<PhysicsComponent>();
			if (bt->getBody())
				m_world->addRigidBody(bt->getBody());
			pc->setBody(std::move(body));
		}
		else if (bt) {
			LOG(LOG_ERROR) << "BulletPhysicsService::initializeItem: failed to initialize body for item " + item->getPathName();			
		}
		return;
	}

	void BulletPhysicsService::removeItem(rhea::Item* item)
	{
		item->deleteComponent<PhysicsComponent>();
	}

	void BulletPhysicsService::onInit()
	{ 
		//collision configuration contains default setup for memory, collision setup.
		btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

		///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

		///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

		m_world = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

		m_world->setGravity(btVector3(0.0f, -0.8f, 0.0f));

		m_app->getUnitTree()->registerEventHandler("scene_activate", this, [&](rhea::Event* ev, rhea::Unit* tree)->bool {
			setScene(doCast<rhea::Scene>(ev->getTarget()));
			return false;
			});
		m_app->getUnitTree()->registerEventHandler("scene_deactivate", this, [&](rhea::Event* ev, rhea::Unit* tree)->bool {
			clearScene(doCast<rhea::Scene>(ev->getTarget()));
			return false;
			});

	}
	
	rhea::Unit* BulletPhysicsService::ray(math::Vector3 origin, math::Vector3 direction)
	{
		btVector3 btRayFrom = btVector3(origin.x, origin.y, origin.z);
		btVector3 btRayTo = btVector3(direction.x, direction.y, direction.z);
		
		btCollisionWorld::ClosestRayResultCallback rayCallback(btRayFrom, btRayTo);
		m_world->rayTest(btRayFrom, btRayTo, rayCallback);
		if (rayCallback.hasHit()) {
			rhea::Item* item = reinterpret_cast<rhea::Item*>(rayCallback.m_collisionObject->getUserPointer());
			//btRigidBody* pBody = btRigidBody::upcast(rayCallback.m_collisionObject);
			if (item) {
#ifdef PICK3D_DEBUG
				LOG(LOG_ERROR) << "item:" + item->getPathName() + " hit";
#endif
				return item;
			}
		}
		return nullptr;
	}

	std::unique_ptr<rhea::PhysicsBody>
		BulletPhysicsService::createBody(const rhea::PhysicsBodyDesc& desc, rhea::Item* item)
	{
		if (desc.kind == "Meworla/RheaPhysics:Character")
			return std::make_unique<BulletPhysicsCharacter>(m_world, item, desc);

		if (desc.kind == "Meworla/RheaPhysics:HeightMap")
			return std::make_unique<BulletHeightMap>(m_world, item, desc);

		if (desc.kind == "Meworla/RheaPhysics:RigidBody")
			return std::make_unique<BulletPhysicsBody>(m_world, item, desc);

		return nullptr;
	}

	void BulletPhysicsService::dumpStats() const
	{
		LOG(LOG_INFO) << "BulletPhysicsService Stats:";
		if (m_world) {
			LOG(LOG_INFO) << "World exists";
		} else {
			LOG(LOG_INFO) << "World does not exist";
		}
		LOG(LOG_INFO) << "Number of collision objects: " << (m_world ? m_world->getNumCollisionObjects() : 0);
	}
}