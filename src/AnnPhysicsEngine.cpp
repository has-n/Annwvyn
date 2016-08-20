#include "stdafx.h"
#include "AnnPhysicsEngine.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

AnnPhysicsEngine::AnnPhysicsEngine(Ogre::SceneNode * rootNode, shared_ptr<AnnPlayer> player, AnnGameObjectList & objects, AnnTriggerObjectList & triggers) : AnnSubSystem("PhysicsEngie"),
playerObject(player),
gameObjects(objects),
triggerObjects(triggers),
Broadphase(nullptr),
CollisionConfiguration(nullptr),
Solver(nullptr),
DynamicsWorld(nullptr),
debugDrawer(nullptr),
playerRigidBodyState(nullptr)
{
	//Initialize the Bullet world
	Broadphase = new btDbvtBroadphase();
	CollisionConfiguration = new btDefaultCollisionConfiguration();
	Dispatcher = new btCollisionDispatcher(CollisionConfiguration);
	Solver = new btSequentialImpulseConstraintSolver();
	DynamicsWorld = new btDiscreteDynamicsWorld(Dispatcher, Broadphase, Solver, CollisionConfiguration);
	AnnDebug() << "btDiscreteDynamicsWorld instantiated";

	//Set gravity vector
	DynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
	AnnDebug("Gravity vector = (0, -9.81f, 0)");

	debugPhysics = false;//by default
	debugDrawer = new BtOgre::DebugDrawer(rootNode, DynamicsWorld);
	DynamicsWorld->setDebugDrawer(debugDrawer);
}

AnnPhysicsEngine::~AnnPhysicsEngine()
{
	delete DynamicsWorld;
	delete Broadphase;
	delete CollisionConfiguration;
	delete Dispatcher;
	delete Solver;
	delete debugDrawer;
}

void AnnPhysicsEngine::addPlayerPhysicalBodyToDynamicsWorld()
{
	assert(playerObject->getBody());
	//TODO define name for the bullet's collision masks
	DynamicsWorld->addRigidBody(playerObject->getBody(), MASK(0), MASK(1));
}

void AnnPhysicsEngine::createPlayerPhysicalVirtualBody(Ogre::SceneNode* node)
{
	AnnDebug() << "createPlayerPhysicalVirtualBody";
	//Player need to have a shape (capsule)
	assert(playerObject->getShape());

	//Create a rigid body state through BtOgre
	if (playerRigidBodyState) delete playerRigidBodyState;
	playerRigidBodyState = new BtOgre::RigidBodyState(node);

	//Get inertia vector
	btVector3 inertia;
	playerObject->getShape()->calculateLocalInertia(playerObject->getMass(), inertia);

	//Set the body to the player
	playerObject->setBody(new btRigidBody(
		playerObject->getMass(),
		playerRigidBodyState,
		playerObject->getShape(),
		inertia));
}

void AnnPhysicsEngine::createVirtualBodyShape()
{
	assert(playerObject);
	float radius(0.125f);
	playerObject->setShape(new btCapsuleShape(radius, playerObject->getEyesHeight() - 2 * radius));
}

btDiscreteDynamicsWorld* AnnPhysicsEngine::getWorld()
{
	return DynamicsWorld;
}

void AnnPhysicsEngine::step(float delta)
{
	//AnnDebug() << "sepSimulation with delta = " << delta;
	DynamicsWorld->stepSimulation(delta/*, 3, 1.0f / 90.0f*/);
}

void AnnPhysicsEngine::stepDebugDrawer()
{
	if (debugPhysics)
		debugDrawer->step();
}

void AnnPhysicsEngine::processCollisionTesting(AnnGameObjectList& objects)
{
	//TODO make a typedeff for getting off the uglyness here 
	std::vector<struct collisionTest*> pairs;

	//get all collision mask
	auto objectIteartor(objects.begin());
	for (size_t i = 0; i < objects.size(); i++)
	{
		std::vector<struct collisionTest*> onThisObject = (*objectIteartor++)->getCollisionMask();

		for (size_t j = 0; j < onThisObject.size(); j++)
			pairs.push_back(onThisObject[j]);
	}

	//Reset the value before extracting data 
	for (auto pair : pairs)
		pair->collisionState = false;

	//process for each maniflod
	int numManifolds = Dispatcher->getNumManifolds();

	//m is manifold identifier
	for (int m(0); m < numManifolds; m++)
	{
		btPersistentManifold* contactManifold =
			DynamicsWorld->getDispatcher()->getManifoldByIndexInternal(m);

		const btCollisionObject* obA = (btCollisionObject*)contactManifold->getBody0();
		const btCollisionObject* obB = (btCollisionObject*)contactManifold->getBody1();

		//Just get the address of the collision objects
		void* pair1 = (void*)obA;
		void* pair2 = (void*)obB;

		//for each known pair on the collision feedback system
		for (size_t p = 0; p < pairs.size(); p++)
		{
			//Get the bodies from the manifold
			void* body1 = (void*)pairs[p]->Object->getBody();
			void* body2 = (void*)pairs[p]->Receiver->getBody();

			//If there is a collision in either way
			if ((pair1 == body1 && pair2 == body2) ||
				(pair2 == body1 && pair1 == body2))
			{
				if (contactManifold->getNumContacts() > 0)
					pairs[p]->collisionState = true;
				else
					pairs[p]->collisionState = false;
				break;
			}
		}
	}
}

void AnnPhysicsEngine::removeRigidBody(btRigidBody* body)
{
	AnnDebug() << "Removing " << body << " Form physics simulation";
	if (body)
		DynamicsWorld->removeRigidBody(body);
}

void AnnPhysicsEngine::initPlayerPhysics(Ogre::SceneNode* node)
{
	AnnDebug() << "Initializing player's physics " << playerObject->getMass() << "Kg ~" << playerObject->getEyesHeight();
	createVirtualBodyShape();
	createPlayerPhysicalVirtualBody(node);
	addPlayerPhysicalBodyToDynamicsWorld();
}

void AnnPhysicsEngine::setDebugPhysics(bool state)
{
	debugPhysics = state;
}

void AnnPhysicsEngine::processTriggersContacts()
{
	for (auto trigger : triggerObjects)
	{
		auto current(trigger);
		if (current->computeVolumetricTest(playerObject))
		{
			current->setContactInformation(true);
			current->atContact();
		}
		else
		{
			current->setContactInformation(false);
		}

		if ((!current->lastFrameContactWithPlayer && current->contactWithPlayer)
			|| (current->lastFrameContactWithPlayer && !current->contactWithPlayer))
			AnnGetEventManager()->spatialTrigger(current);
	}
}

void AnnPhysicsEngine::changeGravity(AnnVect3 gravity)
{
	DynamicsWorld->setGravity(gravity.getBtVector());
}

void AnnPhysicsEngine::update()
{
	processCollisionTesting(gameObjects);
	processTriggersContacts();
	stepDebugDrawer();
	step(AnnGetEngine()->getFrameTime());
}
