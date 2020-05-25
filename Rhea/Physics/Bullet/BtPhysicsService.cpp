#include "BtPhysicsService.h"
#include "Controller/BasicController.h"
#include "Item.h"
#include "Scene.h"
#include "BtBody.h"
#include "BtHeightMap.h"
#include "BtDebugDraw.h"

#include "btBulletDynamicsCommon.h"

namespace Rhea
{
	BulletPhysicsService::BulletPhysicsService(Application* app) :
		m_world(nullptr),
		m_scene(nullptr),
		m_debug(false),
		PhysicsService(app, "BulletPhysicsService")
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

		for (auto it = items.begin(); it != items.end(); ++it) {
			Item* item = *it;
			BulletPhysicsBody* body = static_cast<BulletPhysicsBody*>(item->getPhysicsBody());
			if (body)
				body->doPhysics(delta);
		}

		m_world->stepSimulation(delta, 1);

		for (int j = m_world->getNumCollisionObjects() - 1; j >= 0; j--)
		{
			btCollisionObject* obj = m_world->getCollisionObjectArray()[j];
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
			Item* item = static_cast<Item*>(obj->getUserPointer());
			// update position
			if (item) {
				Poseidon::Position* position = item->getNode()->get_position();

				position->setPosition(Poseidon::Vector3(float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ())));
				btQuaternion rot = trans.getRotation();
				position->setOrientation(Poseidon::Quaternion(rot.getW(), rot.getX(), rot.getY(), rot.getZ()));

				item->setPosition(*position);				
			}
		}
	}
	
	void BulletPhysicsService::setScene(Scene* scene)
	{
		btAlignedObjectArray<btCollisionShape*> collisionShapes;
		bool defaultGround = scene->getValue("ground");
		m_debug = scene->getValue("debug_physics");

		if (m_debug) {
			BulletDebugDraw* debug = new BulletDebugDraw(m_app, m_world);
		}

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

			if (item->getType() == "//meworla/terrain") {
				BulletHeightMap* hmap = new BulletHeightMap(item);
				item->addElement("physics", hmap);
				m_world->addRigidBody(hmap->getBody());
				
			}
			else if (item->getType() == "//meworla/camera") {
				// make camera without physics	
				m_item = item;
			}
			else {
				BulletPhysicsBody* body = new BulletPhysicsBody(item);
				item->addElement("physics", body);
				m_world->addRigidBody(body->getBody());
			}
		}
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
	}
}