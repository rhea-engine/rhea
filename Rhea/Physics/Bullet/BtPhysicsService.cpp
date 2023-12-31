#include "BtPhysicsService.h"
#include "Controller/BasicController.h"
#include "Fabrics/3D/Item.h"
#include "Fabrics/Core/Scene.h"
#include "BtBody.h"
#include "BtHeightMap.h"
#include "BtDebugDraw.h"
#include "BtGhost.h"
#include "BtCharacter.h"
#include "Fabrics/Terrain/Terrain.h"

#include "Poseidon/Core.h"
#include "Poseidon/Viewport.h"
#include "Poseidon/SceneManager.h"
#include <Hippios/VertexBuffer.h>
#include <Poseidon/Shader.h>
#include <Poseidon/RenderContext.h>

#include <bullet/src/btBulletDynamicsCommon.h>
#include <Psio/Log.h>
#include "Application.h"
#include "UnitTree.h"

#include <assert.h>

namespace Rhea
{
	BulletPhysicsService::BulletPhysicsService(std::string name, std::string type_name) :
		m_world(nullptr),
		m_scene(nullptr),
		PhysicsService("BulletPhysicsService", type_name)
	{
	}

	void BulletPhysicsService::step(float delta)
	{
		if (!m_world || !m_scene) {		
			assert(false);
			return;
		}
		PhysicsService::step(delta);

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

			// update position for dynamic objects from physics engine
			if (item && static_cast<PhysicsBody*>(item->getPhysicsBody())->isDynamic()) {

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
			if (item->getElement("physics"))
				continue;

			initializeItem(item);
		}
	}

	void BulletPhysicsService::initializeItem(Item* item)
	{
		if (item->getTypeName() == "//meworla/terrain") {
			BulletHeightMap* hmap = new BulletHeightMap(item);
			item->addElement("physics", hmap);
			m_world->addRigidBody(hmap->getBody());
			// TODO currently simply set the last terrain to active
			m_terrain = doCast<Terrain>(item);
		}
		else if (item->getTypeName() == "//meworla/camera") {
			// make camera without physics	
			m_item = item;
		}
		else if (item->getTypeName() == "//meworla/character") {
			BulletPhysicsCharacter* body = new BulletPhysicsCharacter(m_world, item);
			body->init();
			item->addElement("physics", body);
		}
		else if (item->getTypeName() == "//meworla/widget")
			return;
		else if (item->getTypeName() == "//meworla/textwidget")
			return;
		else {
			BulletPhysicsBody* body = new BulletPhysicsBody(m_world, item);
			if (body->init()) {
				item->addElement("physics", body);
				m_world->addRigidBody(body->getBody());
			}
		}
	}

	void BulletPhysicsService::removeItem(Item* item)
	{
		item->addElement("physics", nullptr);
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
}