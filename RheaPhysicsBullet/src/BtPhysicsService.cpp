#include <RheaPhysicsBullet/RheaPhysicsBulletService.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Core/Scene.h>
#include "BtBody.h"
#include "BtGhost.h"
#include "BtCharacter.h"

#include <bullet/src/btBulletDynamicsCommon.h>
#include <Psio/Log.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>

#include <assert.h>
#include <RheaPhysics/PhysicsComponent.h>
#include <Rhea/Core/Camera.h>

#include "BulletHeightMap.h"
#include <Rhea/Physics/PhysicsManagerService.h>
#include <Rhea/Physics/PhysicsEvent.h>

namespace Rhea
{
	BulletPhysicsService::BulletPhysicsService(std::string name, std::string type_name) :
		m_world(nullptr),
		m_scene(nullptr),
		m_debug_drawer(nullptr),
		PhysicsService("BulletPhysicsService", type_name)
	{
	}

	void BulletPhysicsService::onPhysicsStep(float delta)
	{
		if (!m_world || !m_scene) {		
			assert(false);
			return;
		}
		std::unique_ptr<Rhea::PhysicsEvent> ev = std::make_unique<Rhea::PhysicsEvent>("physics_pre", delta);
		sendEvent(std::string("pre_physics"), ev.get());

		std::vector<Item*> items = m_scene->getItems();

		/*
		for (auto it = items.begin(); it != items.end(); ++it) {
			Item* item = *it;
			BulletPhysicsBody* body = static_cast<BulletPhysicsBody*>(item->getPhysicsBody());
			if (body)
				body->doPhysics(delta, m_terrain);
		}
		*/

		m_world->stepSimulation(delta, 1);

		for (int j = m_world->getNumCollisionObjects() - 1; j >= 0; j--)
		{
			btCollisionObject* obj = m_world->getCollisionObjectArray()[j];
			Item* item = doCast<Item>((SimpleParser::Variant*)obj->getUserPointer());

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
				Rhea::Position* position = item->getTransform()->get_position();

				position->setPosition(Math::Vector3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ())));
				btQuaternion rot = trans.getRotation();
				position->setOrientation(Math::Quaternion(rot.getW(), rot.getX(), rot.getY(), rot.getZ()));

				item->setPosition(*position);				
			}
		}
	}
	
	void BulletPhysicsService::setScene(Scene* scene)
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
			btRigidBody* body = new btRigidBody(rbInfo);

			//add the body to the dynamics world
			m_world->addRigidBody(body);
		}

		// get items of scene
		m_scene = scene;
		std::vector<Item*> items = m_scene->getItems();

		for (auto it = items.begin(); it != items.end(); ++it) {
			Item* item = *it;
			if (item->getComponent<PhysicsComponent>())
				continue;

			initializeItem(item);
		}
	}

	void BulletPhysicsService::initializeItem(Item* item)
	{
		auto* mgr = doCast<PhysicsManagerService>(m_app->getService("physics_mgr"));
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
		return;
	}

	void BulletPhysicsService::removeItem(Item* item)
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

		m_app->getUnitTree()->registerEventHandler("scene_activate", this, [&](Event* ev, Unit* tree)->bool {
			setScene(doCast<Scene>(ev->getTarget()));
			return false;
			});
	}
	
	Unit* BulletPhysicsService::ray(Math::Vector3 origin, Math::Vector3 direction)
	{
		btVector3 btRayFrom = btVector3(origin.x, origin.y, origin.z);
		btVector3 btRayTo = btVector3(direction.x, direction.y, direction.z);
		
		btCollisionWorld::ClosestRayResultCallback rayCallback(btRayFrom, btRayTo);
		m_world->rayTest(btRayFrom, btRayTo, rayCallback);
		if (rayCallback.hasHit()) {
			Rhea::Item* item = reinterpret_cast<Rhea::Item*>(rayCallback.m_collisionObject->getUserPointer());
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

	std::unique_ptr<PhysicsBody>
		BulletPhysicsService::createBody(const PhysicsBodyDesc& desc, Item* item)
	{
		if (desc.kind == "Meworla/RheaPhysics:Character")
			return std::make_unique<BulletPhysicsCharacter>(m_world, item, desc);

		if (desc.kind == "Meworla/RheaPhysics:HeightMap")
			return std::make_unique<BulletHeightMap>(m_world, item, desc);

		if (desc.kind == "Meworla/RheaPhysics:RigidBody")
			return std::make_unique<BulletPhysicsBody>(m_world, item, desc);

		return nullptr;
	}

	void BulletPhysicsService::setDebugDrawer(btIDebugDraw* drawer)
	{
		if (drawer)
			m_world->setDebugDrawer(drawer);
	}
}