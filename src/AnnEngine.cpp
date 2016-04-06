#include "stdafx.h"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"
using namespace Annwvyn;

AnnEngine* AnnEngine::singleton(NULL);
AnnConsole* AnnEngine::onScreenConsole(NULL);

AnnEngine* AnnEngine::Instance()
{
	return singleton;
}

std::string AnnEngine::getAnnwvynVersion()
{
	std::stringstream version;
	version << ANN_MAJOR << "." << ANN_MINOR << "." << ANN_PATCH;
	return version.str();
}

void AnnEngine::startGameplayLoop()
{
	while(refresh());
}
	
AnnEngine::AnnEngine(const char title[]) :
	eventManager(NULL),
	levelManager(NULL),
	povNode(NULL),	
	defaultEventListener(NULL)
{
	if(singleton) 
	{
		log("Can't create 2 instances of the engine!");
		exit(ANN_ERR_MEMORY);
	}
	srand(time(nullptr));
	singleton = this;

	//Launching initialisation routines : 
	//All Ogre related critical component is done inside the OgreOculusRenderer class. 
	renderer = new OgreOculusRender(title);
	renderer->initLibraries("Annwvyn.log");
	player = new AnnPlayer;
	renderer->getOgreConfig();
	renderer->createWindow();
	renderer->initScene();
	renderer->initCameras();
	renderer->setCamerasNearClippingDistance(0.15f);
	renderer->initRttRendering();
	SceneManager = renderer->getSceneManager();

	log("OGRE Object-oriented Graphical Rendering Engine initialized", true);

	renderer->showMonscopicView();

	log("Setup Annwvyn's subsystems");
	eventManager = new AnnEventManager(renderer->getWindow());
	physicsEngine = new AnnPhysicsEngine(getSceneManager()->getRootSceneNode());
	AudioEngine = new AnnAudioEngine;
	levelManager = new AnnLevelManager;
	filesystemManager = new AnnFilesystemManager;
	filesystemManager->setSaveDirectoryName(title);

	log("===================================================", false);
	log("Annwvyn Game Engine - Step into the Other World    ", false);
	log("Free/Libre Game Engine designed for Virtual Reality", false);
	log("Version : " + getAnnwvynVersion()                   , false);
	log("===================================================" , false);
}

AnnEngine::~AnnEngine()
{

	log("Destroying the event manager");
	delete eventManager;
	eventManager = nullptr;

	log("Destroy the levelManager");
	delete levelManager;
	levelManager = nullptr;

	log("Destroying every objects remaining in engine");
	
	log(" Creating the destroing queue;");
	AnnDebug() << " Will destroy " << objects.size() << " remaining objects";
	AnnDebug() << " Will destroy " << triggers.size() << " remaining triggers";
	AnnDebug() << " Will destroy " << lights.size() << " remaining lights";

	AnnGameObject** tmpArrayObj = static_cast<AnnGameObject**>(malloc(sizeof(AnnGameObject*)*objects.size()));
	AnnTriggerObject** tmpArrayTrig = static_cast<AnnTriggerObject**>(malloc(sizeof(AnnTriggerObject*)*triggers.size()));
	AnnLightObject** tmpArrayLight = static_cast<AnnLightObject**>(malloc(sizeof(AnnLightObject*)*lights.size()));
	
	auto objIt = objects.begin();
	auto trigIt = triggers.begin();
	auto lightIt = lights.begin();

	for(size_t i(0); i < objects.size(); i++) tmpArrayObj[i] = *objIt++;;
	for(size_t i(0); i < triggers.size(); i++) tmpArrayTrig[i] = *trigIt++;
	for(size_t i(0); i < lights.size(); i++) tmpArrayLight[i] = *lightIt++;

	log("Content of the destroing queue :");
	log("Game Object");
	for(size_t i(0); i < objects.size(); i++)
		AnnDebug() << (void*)tmpArrayObj[i];
	log("Trigger Object");
	for(size_t i(0); i < triggers.size(); i++)
		AnnDebug() << (void*)tmpArrayTrig[i];
	log("Light object");
	for(size_t i(0); i < lights.size(); i++)
		AnnDebug() << (void*)tmpArrayLight[i];

	size_t queueSize;
	queueSize = objects.size();
	for(size_t i(0); i < queueSize; i++)
		destroyGameObject(tmpArrayObj[i]);
	queueSize = triggers.size();
	for(size_t i(0); i < queueSize; i++)
		destroyTriggerObject(tmpArrayTrig[i]);
	queueSize = lights.size();
	for(size_t i(0); i < queueSize; i++)
		destroyLightObject(tmpArrayLight[i]);


	log("Destroing the deletion queues");
	free(tmpArrayObj);
	free(tmpArrayTrig);
	free(tmpArrayLight);

	log("Clearing object lists");
	objects.clear();
	triggers.clear();
	lights.clear();

	log("Destroying physics engine");
	delete physicsEngine;
	physicsEngine = nullptr;
	log("Destroying Player");
	delete player;
	player = nullptr;
	log("Destroying AudioEngine");
	delete AudioEngine;
	AudioEngine = nullptr;

	log("Game engine sucessfully destroyed.");
	log("Good luck with the real world now! :3");
	delete onScreenConsole;
	onScreenConsole = NULL;
	singleton = NULL;
	delete renderer;
	renderer = nullptr;
}

AnnEventManager* AnnEngine::getEventManager()
{
	return eventManager;
}

AnnLevelManager* AnnEngine::getLevelManager()
{
	return levelManager;
}

AnnPlayer* AnnEngine::getPlayer()
{
	return player;
}

AnnFilesystemManager* AnnEngine::getFileSystemManager()
{
	return filesystemManager;
}

////////////////////////////////////////////////////////// UTILITY
void AnnEngine::log(std::string message, bool flag)
{
	Ogre::String messageForLog;

	if(flag)
		messageForLog += "Annwvyn - ";

	messageForLog += message;
	Ogre::LogManager::getSingleton().logMessage(messageForLog);
	if(onScreenConsole)
		onScreenConsole->append(message);
}

void AnnEngine::useDefaultEventListener()
{
	if(!eventManager) return; 
	log("Reconfiguring the engine to use the default event listener");
	log("This unregister any current listener in use!");

	//Remove all event listeners
	eventManager->removeListener();

	//If the event listenre isn't allready initialized, allocate one
	if(!defaultEventListener)
		defaultEventListener = new AnnDefaultEventListener;

	//Set the default event listener to the event manager
	eventManager->addListener(defaultEventListener);
}

AnnDefaultEventListener* AnnEngine::getInEngineDefaultListener()
{
	return defaultEventListener;
}

void AnnEngine::initPlayerPhysics()
{
	physicsEngine->initPlayerPhysics(player, povNode);
}

void AnnEngine::loadZip(const char path[], const char resourceGroupName[])
{
	log("Loading resources from Zip archive :");
	log(path, false);
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path, "Zip", resourceGroupName);
}

void AnnEngine::loadDir(const char path[], const char resourceGroupName[])
{
	log("Loading resources from Filesystem directory :");
	log(path, false);
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path, "FileSystem", resourceGroupName);
}

void AnnEngine::loadResFile(const char path[])
{
	AnnDebug() << "Loading resource file : " << path;
	renderer->loadReseourceFile(path);
}

void AnnEngine::initResources()
{
	addDefaultResourceLocaton();
	renderer->initAllResources();
	log("Resources initialized");
}

void AnnEngine::addDefaultResourceLocaton()
{
	log("Adding Annwvyn CORE resources");
	loadDir("media");
	loadZip("media/CORE.zip");
}

void AnnEngine::oculusInit()
{   
	log("Init Oculus rendering system");
	renderer->initOculus();
	povNode = renderer->getCameraInformationNode();
	povNode->setPosition(player->getPosition() + 
		AnnVect3(0.0f, player->getEyesHeight(), 0.0f));
	onScreenConsole = new AnnConsole();
	//This will populate swap texture and turn on the rift display earlier
	renderer->updateTracking();
	renderer->renderAndSubmitFrame();
}

AnnGameObject* AnnEngine::createGameObject(const char entityName[], AnnGameObject* obj)
{
	log("Creatig a game object from the entity " + std::string(entityName));

	if(std::string(entityName).empty())
	{
		log("Hey! what are you trying to do here? Please specify a non empty string for entityName !");
		delete obj;
		return NULL;
	}

	Ogre::Entity* ent = SceneManager->createEntity(entityName);
	Ogre::SceneNode* node = SceneManager->getRootSceneNode()->createChildSceneNode();

	node->attachObject(ent);
	obj->setNode(node);
	obj->setEntity(ent);
	obj->setBulletDynamicsWorld(physicsEngine->getWorld());
	obj->postInit(); //Run post init directives

	objects.push_back(obj); //keep addreAnnDebug() in list

	AnnDebug() << "The object " << entityName << " has been created. Annwvyn memory address " << obj;  

	return obj;
}

void AnnEngine::destroyTriggerObject(AnnTriggerObject* object)
{
	triggers.remove(object);
	AnnDebug() << "Destroy trigger : " << (void*)object;
	delete object;
}

bool AnnEngine::destroyGameObject(Annwvyn::AnnGameObject* object)
{
	AnnDebug() << "Destroy call : " << object;
	if(!object) return false;
	bool returnCode(false);
	//TODO: remove the need to mark objects as NULL in this array before being able to clear them.
	for(auto it = objects.begin(); it != objects.end(); it++)
    {
		if(!*it) continue;
		(*it)->stopGettingCollisionWith(object); 
		if(*it == object)
		{
			returnCode = true;
			*it = NULL;

			Ogre::SceneNode* node = object->getNode();
			node->getParent()->removeChild(node);
			size_t nbObject(node->numAttachedObjects());
			std::vector<Ogre::MovableObject*> attachedObject;

			for(unsigned short i(0); i < nbObject; i++)
				attachedObject.push_back(node->getAttachedObject(i));

			node->detachAllObjects();

			auto attachedIterator(attachedObject.begin());
			while(attachedIterator!= attachedObject.end())
				SceneManager->destroyMovableObject(*attachedIterator++);

			physicsEngine->removeRigidBody(object->getBody());
			SceneManager->destroySceneNode(node);
			delete object;
		}
	}

	//Clear everything equals to "NULL" on the vector
	objects.remove(NULL);

	return returnCode;
}

AnnLightObject* AnnEngine::createLightObject()
{
	log("Creating a light");
	AnnLightObject* Light = new AnnLightObject(SceneManager->createLight());
	Light->light->setType(Ogre::Light::LT_POINT);
	lights.push_back(Light);
	return Light;
}

void AnnEngine::destroyLightObject(AnnLightObject* object)
{
	if(object)
		SceneManager->destroyLight(object->light);

	//Forget that this light existed
	lights.remove(object);
	delete object;
}

bool AnnEngine::requestStop()
{
	//pres ESC to quit. Stupid but efficient. I like that.
	if(isKeyDown(OIS::KC_ESCAPE))
		return true;
	return false;
}

bool AnnEngine::refresh()
{
	//Get the rendering delta time (should be roughly equals to 1/desiredFramerate in seconds)
	updateTime = renderer->getUpdateTime();
	physicsEngine->step(updateTime);
	player->engineUpdate(updateTime);

	//Process some logic to extract basic informations (theses should be done within the eventManager).
	physicsEngine->processCollisionTesting(objects);
	physicsEngine->processTriggersContacts(player, triggers);

	//Update the event system
	eventManager->update(); 
	levelManager->update();

	//Run animations and update OpenAL sources position
	for(auto gameObject : objects)
	{
		gameObject->addAnimationTime(updateTime);
		gameObject->updateOpenAlPos();
		gameObject->atRefresh();
	}

	//Update camera from player
	syncPov();
	renderer->updateTracking();
	//Audio
	AudioEngine->updateListenerPos(renderer->returnPose.position);
	AudioEngine->updateListenerOrient(renderer->returnPose.orientation);

	if(onScreenConsole->needUpdate()) 
		onScreenConsole->update();
	physicsEngine->stepDebugDrawer();
	renderer->renderAndSubmitFrame();

	return !requestStop();
}

inline void AnnEngine::syncPov()
{
	povNode->setPosition(player->getPosition());
	povNode->setOrientation(player->getOrientation().toQuaternion());
}


inline bool AnnEngine::isKeyDown(OIS::KeyCode key)
{
	if(!eventManager) return false;
	return eventManager->Keyboard->isKeyDown(key);
}

AnnTriggerObject* AnnEngine::createTriggerObject(AnnTriggerObject* object)
{
	assert(object);
	log("Creating a trigger object");
	triggers.push_back(object);
	object->postInit();
	return object;
}

AnnGameObject* AnnEngine::playerLookingAt()
{
	//Origin vector of the ray
	AnnVect3 Orig(getPoseFromOOR().position);

	//Caltulate direction Vector of the ray to be the midpont camera optical axis
	AnnVect3 LookAt(AnnQuaternion(getPoseFromOOR().orientation).getAtVector());

	//create ray
	Ogre::Ray ray(Orig, LookAt);

	//create query
	Ogre::RaySceneQuery* raySceneQuery(SceneManager->createRayQuery(ray));
	raySceneQuery->setSortByDistance(true);

	//execute and get the results
	Ogre::RaySceneQueryResult& result(raySceneQuery->execute());

	//read the result list
	for(auto it(result.begin()); it != result.end(); it++)
		if(it->movable && it->movable->getMovableType() == "Entity")
			return getFromNode(it->movable->getParentSceneNode());//Get the AnnGameObject that is attached to this SceneNode	

	return nullptr; //means that we don't know what the player is looking at.
}

void AnnEngine::resetOculusOrientation()
{
	log("Reseting the base direction of player's head");
	renderer->recenter();
}

Annwvyn::AnnGameObject* AnnEngine::getFromNode(Ogre::SceneNode* node)
{
	if(!node)
	{
		log("Plese do not try to identify a NULL");
		return NULL;
	}
	AnnDebug() << "Trying to identify object at address " << (void*)node;

	//This methods only test memory address
	for(auto object : objects)
		if((void*)object->getNode() == (void*)node)
			return object;
	AnnDebug() << "The object " << (void*)node << " doesn't belong to any AnnGameObject";

	return NULL;
}

////////////////////////////////////////////////////////// GETTERS
Ogre::SceneNode* AnnEngine::getCamera()
{
	return povNode;
}

AnnAudioEngine* AnnEngine::getAudioEngine()
{
	return AudioEngine;
}

AnnPhysicsEngine* AnnEngine::getPhysicsEngine()
{
	return physicsEngine;
}


Ogre::SceneManager* AnnEngine::getSceneManager()
{
	return SceneManager;
}

unsigned long AnnEngine::getTimeFromStartUp()
{
	return renderer->getTimer()->getMilliseconds();
}

double AnnEngine::getTimeFromStartupSeconds()
{
	return static_cast<double>(getTimeFromStartUp())/1000.0;
}

double AnnEngine::getFrameTime()
{
	return updateTime;
}

////////////////////////////////////////////////////////// SETTERS
void AnnEngine::setDebugPhysicState(bool state)
{
	assert(physicsEngine);
	log("Activating debug drawing for physics engine");
	physicsEngine->setDebugPhysics(state);
}

void AnnEngine::setAmbiantLight(AnnColor color)
{
	AnnDebug() << "Setting the ambiant light to color " << color; 
	SceneManager->setAmbientLight(color.getOgreColor());
}

void AnnEngine::setSkyDomeMaterial(bool activate, const char materialName[], float curvature, float tiling)
{
	log("Setting skydome from material"); log(materialName, false);
	SceneManager->setSkyDome(activate, materialName, curvature, tiling);
}

void AnnEngine::setSkyBoxMaterial(bool activate, const char materialName[], float distance, bool renderedFirst)
{
	log("Setting skybox from material"); log(materialName, false);
	SceneManager->setSkyBox(activate, materialName, distance, renderedFirst);
}

void AnnEngine::setWorldBackgroundColor(AnnColor v)
{
		AnnDebug() << "Setting the backgroud world color " << v;
		renderer->changeViewportBackgroundColor(v.getOgreColor()); 
}

void AnnEngine::removeSkyDome()
{
	log("Disabeling skydome");
	SceneManager->setSkyDomeEnabled(false);
}

void AnnEngine::removeSkyBox()
{
	log("Disabeling skybox");
	SceneManager->setSkyBoxEnabled(false);
}

void AnnEngine::setNearClippingDistance(Ogre::Real nearClippingDistance)
{
	AnnDebug() << "Setting the near clipping distance to " << nearClippingDistance;

	if(renderer)
		renderer->setCamerasNearClippingDistance(nearClippingDistance);
}

void AnnEngine::resetPlayerPhysics()
{
	if(!player->hasPhysics()) return;
	log("Reset player's physics");

	//Remove the player's rigidbody from the world
	physicsEngine->getWorld()->removeRigidBody(player->getBody());
	
	//We don't need that body anymore...
	delete player->getBody();
	//prevent memory access to unallocated address
	player->setBody(NULL);

	//Put everything back in order
	povNode->setPosition(player->getPosition());
	physicsEngine->createPlayerPhysicalVirtualBody(player, povNode);
	physicsEngine->addPlayerPhysicalBodyToDynamicsWorld(player);
}

OgrePose AnnEngine::getPoseFromOOR()
{
	if(renderer)
		return renderer->returnPose;
	return OgrePose();
}

void AnnEngine::openConsole()
{
#ifdef _WIN32
	int outHandle, errHandle, inHandle;
    FILE *outFile, *errFile, *inFile;
    AllocConsole();
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = 9999;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

    outHandle = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
    errHandle = _open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);
    inHandle = _open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT );

    outFile = _fdopen(outHandle, "w" );
    errFile = _fdopen(errHandle, "w");
    inFile =  _fdopen(inHandle, "r");

    *stdout = *outFile;
    *stderr = *errFile;
    *stdin = *inFile;

    setvbuf(stdout, NULL, _IONBF, 0 );
    setvbuf(stderr, NULL, _IONBF, 0 );
    setvbuf(stdin, NULL, _IONBF, 0 );

    std::ios::sync_with_stdio();
#endif
}

void AnnEngine::toogleOnScreenConsole()
{
	if(onScreenConsole) onScreenConsole->toogle();
}

void AnnEngine::toogleOculusPerfHUD()
{
	if(renderer) renderer->cycleOculusHUD();
}