#include "AnnEngine.hpp"

using namespace Annwvyn;

AnnEngine* AnnEngine::singleton(NULL);

AnnEngine* AnnEngine::Instance()
{
	return singleton;
}

AnnEngine::AnnEngine(const char title[])
{
	lastFrameTimeCode = 0;
	currentFrameTimeCode = 0;

	//Make the necessary singleton initialization. 
	if(singleton) abort();
	singleton = this;

	m_CameraReference = NULL;

	//block ressources loading for now
	readyForLoadingRessources = false;

	//This structure handle player's body parameters
	player = new AnnPlayer;
	defaultEventListener = NULL;

	//Launching initialisation routines : 
	//All Ogre related critical component is done inside the OgreOculusRenderer class. 
	oor = new OgreOculusRender(title);
	oor->initLibraries("Annwvyn.log");
	oor->getOgreConfig();
	oor->createWindow();
	oor->initScene();
	oor->initCameras();
	oor->setCamerasNearClippingDistance(0.15f);
	oor->initRttRendering();
	m_SceneManager = oor->getSceneManager();
	m_Window = oor->getWindow();

	readyForLoadingRessources = true;
	log("OGRE Object-oriented Graphical Rendering Engine initialized", true);

//We use OIS to catch all user inputs
#ifdef __gnu_linux__

	//Here's a little hack to save the X11 keyboard layout on Linux, then set it to a standard QWERTY
	//Under windows the keycode match the standard US QWERTY layout. Under linux they are converted to whatever you're using.
	//I use a French AZERTY keyboard layout so it's not that bad. If you have a greek keyboard you're out of luck...
	//So, assuming that the only program that has the focus is the Annwvyn powered application, we can just switch the layout to US 
	//then switch back to the original layout.
	x11LayoutAtStartup = "unknown";

	log("we are running on linux. getting x11 keyboard layout from the system");
	FILE* layout = popen("echo $(setxkbmap -query | grep layout | cut -d : -f 2 )","r");
	char* buffer = static_cast<char *>(malloc(128*sizeof(char)));

	if(layout && buffer)
	{
		fscanf(layout, "%s", buffer);
		x11LayoutAtStartup = std::string(buffer);

		log("Saving keyboard layout for shutdown.");
		log("saved layout="+x11LayoutAtStartup, false);
	}

	free(buffer);
	fclose(layout);

	buffer = NULL;
	layout = NULL;
	system("setxkbmap us");
#endif

	log("Setup event system");
	eventManager = new AnnEventManager(m_Window);

	log("Setup physics engine");
	physicsEngine = new AnnPhysicsEngine(getSceneManager()->getRootSceneNode());
		
	log("Setup audio engine");
	AudioEngine = new AnnAudioEngine;

	//Setting up the Visual Body management 
	VisualBody = NULL;
	VisualBodyAnimation = NULL;
	VisualBodyAnchor = NULL;

	refVisualBody = Ogre::Quaternion::IDENTITY;
	log("---------------------------------------------------", false);
	log("Annwvyn Game Engine - Step into the Other World   ", false);
	log("Designed for Virtual Reality                      ", false);
	log("---------------------------------------------------", false);
}


AnnEngine::~AnnEngine()
{
	log("Stopping the event manager");
	delete eventManager;

	//All AnnGameObject
	log("Destroying every objects remaining in engine");
	for(size_t i(0); i < objects.size(); i++)
	{
		destroyGameObject(objects[i]);
		objects[i] = NULL;  //don't change the size of the vector while iterating throug it
	}
	objects.clear();

	log("Destroying physics engine");
	delete physicsEngine;
	log("Destroying Player");
	delete player;

	log("Destroying AudioEngine");
	delete AudioEngine;

#ifdef __gnu_linux__
	log("setting back the keyboard to " + x11LayoutAtStartup);
	if(x11LayoutAtStartup != "unknown")
	{
		system(std::string("setxkbmap " + x11LayoutAtStartup).c_str());
		log("Done system call to setxkbmap");
	}
#endif

	log("Game engine sucessfully destroyed.");
	log("Good luck with the real world now! :3");
	delete oor;
	singleton = NULL;
}

AnnEventManager* AnnEngine::getEventManager()
{
	return eventManager;
}

AnnPlayer* AnnEngine::getPlayer()
{
	return player;
}

////////////////////////////////////////////////////////// UTILITY
void AnnEngine::log(std::string message, bool flag)
{
	Ogre::String messageForLog;

	if(flag)
		messageForLog += "Annwvyn - ";

	messageForLog += message;
	Ogre::LogManager::getSingleton().logMessage(messageForLog);
}

void AnnEngine::useDefaultEventListener()
{
	assert(eventManager);

	log("Reconfiguring the engine to use the default event listener");

	//Remove the current event listener (if any)
	eventManager->removeListener();

	//If the event listenre isn't allready initialized, allocate one
	if(!defaultEventListener)
		defaultEventListener = new AnnDefaultEventListener(getPlayer());

	//Set the default event listener to the event manager
	eventManager->addListener(defaultEventListener);
}

AnnDefaultEventListener* AnnEngine::getInEngineDefaultListener()
{
	return defaultEventListener;
}

//Convinient method to the user to call : do it and let go !
void AnnEngine::initPlayerPhysics()
{
	physicsEngine->initPlayerPhysics(player, m_CameraReference);
}

//loading ressources
void AnnEngine::loadZip(const char path[], const char resourceGroupName[])
{
	log("Loading resources from Zip archive :");
	log(path, false);
	if(readyForLoadingRessources)
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path, "Zip", resourceGroupName);
}

void AnnEngine::loadDir(const char path[], const char resourceGroupName[])
{
	log("Loading resources from Filesystem directory :");
	log(path, false);
	if(readyForLoadingRessources)
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path, "FileSystem", resourceGroupName);
}

void AnnEngine::loadResFile(const char path[])
{
	std::stringstream ss; 
	ss << "Loading resource file : " << path;
	log(ss.str());
	oor->loadReseourceFile(path);
}

void AnnEngine::initResources()
{
	addDefaultResourceLocaton();
	oor->initAllResources();
	log("Resources initialized");
}

void AnnEngine::addDefaultResourceLocaton()
{
	log("Adding Annwvyn CORE resources");
	loadDir("media");
	loadZip("media/CORE.zip");
}

//initalize oculus rendering
void AnnEngine::oculusInit(bool fullscreen)
{   
	log("Init Oculus rendering system");
	oor->initOculus(fullscreen);
	m_CameraReference = oor->getCameraInformationNode();
	m_CameraReference->setPosition(player->getPosition() + 
		Ogre::Vector3(0.0f, player->getEyesHeight(), 0.0f));
}

AnnGameObject* AnnEngine::createGameObject(const char entityName[], AnnGameObject* obj)
{
	log("Creatig a game object from the entity");
	log(entityName, false);

	if(std::string(entityName).empty())
	{
		log("Hey! what are you trying to do here? Please specify a non empty string for entityName !");
		delete obj;
		return NULL;
	}

	Ogre::Entity* ent = m_SceneManager->createEntity(entityName);
	Ogre::SceneNode* node = 
	m_SceneManager->getRootSceneNode()->createChildSceneNode();

	node->attachObject(ent);
	obj->setNode(node);
	obj->setEntity(ent);
	obj->setBulletDynamicsWorld(physicsEngine->getWorld());
	obj->postInit(); //Run post init directives

	objects.push_back(obj); //keep address in list

	std::stringstream ss;
	ss << "The object " << entityName << " has been created. Annwvyn memory address " << obj;  
	log(ss.str());
	ss.str("");

	return obj;
}

bool AnnEngine::destroyGameObject(Annwvyn::AnnGameObject* object)
{
	std::stringstream ss;

	ss << "Destroying object " << (void*)object;
	log(ss.str());
	ss.str("");

	bool returnCode(false);
	for(size_t i(0); i < objects.size(); i++)
	{
		ss << "Object " << static_cast<void*>(objects[i]) << " stop collision test";
		log(ss.str());
		ss.str("");

		objects[i]->stopGettingCollisionWith(object);

		if(objects[i] == object)
		{
			log("Object found");

			objects.erase(objects.begin() + i);
			Ogre::SceneNode* node = object->node();

			node->getParent()->removeChild(node);
			physicsEngine->removeRigidBody(object->getBody());

			m_SceneManager->destroySceneNode(node);
			delete object;

			returnCode = true; // found
		}
	}
	return returnCode;
}

Annwvyn::AnnLightObject* AnnEngine::addLight()
{
	log("Creating a light");
	//Actualy here i'm cheating, the AnnLightObjet is a simple typdef to Ogre LightSceneNode
	//I'll add a proper class to do it later
	AnnLightObject* Light = m_SceneManager->createLight();
	Light->setType(Ogre::Light::LT_POINT);
	return Light;
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
	deltaT = oor->getUpdateTime();
	//Call of refresh method
	for(AnnGameObjectVect::iterator it = objects.begin(); it != objects.end(); ++it)
		(*it)->atRefresh();

	//Physics
	physicsEngine->step(deltaT);

	//Test if there is a collision with the ground
	player->engineUpdate(deltaT);

	//Dissmiss health and safety warning
	if(!oor->IsHsDissmissed()) //If not already dissmissed
		for(unsigned char kc(0x00); kc <= 0xED; kc++) //For each keycode available (= every keyboard button)
			if(isKeyDown(static_cast<OIS::KeyCode>(kc))) //if tte selected keycode is available
				{oor->dissmissHS(); break;}	//dissmiss the Health and Safety warning.

	eventManager->update();

	physicsEngine->processCollisionTesting(objects);
	physicsEngine->processTriggersContacts(player, triggers);

	//Animation
	if(VisualBodyAnimation)
		VisualBodyAnimation->addTime(deltaT);
	for(size_t i = 0; i < objects.size(); i++)
		objects[i]->addTime(deltaT);

	//Audio
	AudioEngine->updateListenerPos(oor->returnPose.position);
	AudioEngine->updateListenerOrient(oor->returnPose.orientation);
	for(size_t i = 0; i < objects.size(); i++)
		objects[i]->updateOpenAlPos();

	//Update camera
	m_CameraReference->setPosition(player->getPosition());
	m_CameraReference->setOrientation(/*QuatReference* */ player->getOrientation().toQuaternion());

	physicsEngine->stepDebugDrawer();
	oor->RenderOneFrame();
	return !requestStop();
}

bool AnnEngine::isKeyDown(OIS::KeyCode key)
{
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
	Ogre::Vector3 Orig(getPoseFromOOR().position);

	//Caltulate direction Vector of the ray to be the midpont camera optical axis
	Ogre::Vector3 LookAt(getPoseFromOOR().orientation * Ogre::Vector3::NEGATIVE_UNIT_Z);

	//create ray
	Ogre::Ray ray(Orig, LookAt);

	//create query
	Ogre::RaySceneQuery* raySceneQuery(m_SceneManager->createRayQuery(ray));
	raySceneQuery->setSortByDistance(true);

	//execute and get the results
	Ogre::RaySceneQueryResult& result(raySceneQuery->execute());

	//read the result list
	for(Ogre::RaySceneQueryResult::iterator it(result.begin()); it != result.end(); it++)
		if(it->movable && it->movable->getMovableType() == "Entity")
			return getFromNode(it->movable->getParentSceneNode());//Get the AnnGameObject that is attached to this SceneNode	

	return NULL; //means that we don't know what the player is looking at.
}

void AnnEngine::attachVisualBody(const std::string entityName, float z_offset, bool flip, bool animated , Ogre::Vector3 scale)
{
	log("Attaching visual body :");
	log(entityName);

	Ogre::Entity* ent = m_SceneManager->createEntity(entityName);
	VisualBodyAnchor = m_CameraReference->createChildSceneNode();
	VisualBodyAnchor->attachObject(ent);

	if(flip)
		refVisualBody = Ogre::Quaternion(Ogre::Degree(180), Ogre::Vector3::UNIT_Y);
	else
		refVisualBody = Ogre::Quaternion::IDENTITY;

	visualBody_Zoffset = z_offset;
	VisualBody = ent;

	VisualBodyAnchor->setPosition(0,-player->getEyesHeight(),-visualBody_Zoffset);
	VisualBodyAnchor->setOrientation(refVisualBody);

	if(animated)
	{
		//TODO play idle animation
	}
}

void AnnEngine::resetOculusOrientation()
{
	log("Reseting the base direction of player's head");
	oor->recenter();
}

Annwvyn::AnnGameObject* AnnEngine::getFromNode(Ogre::SceneNode* node)
{
	if(!node) 
	{
		log("Plese do not try to identify a NULL");
		return NULL;
	}
	std::stringstream ss;
	ss << "Trying to identify object at address " << (void*)node;
	log(ss.str());
	ss.str("");

	//This methods only test memory address
	for(size_t i(0); i < objects.size(); i++)
		if((void*)objects[i]->node() == (void*)node)
			return objects[i];
	ss << "The object " << (void*)node << " doesn't belong to any AnnGameObject";

	log(ss.str());
	return NULL;
}

////////////////////////////////////////////////////////// GETTERS
Ogre::SceneNode* AnnEngine::getCamera()
{
	return m_CameraReference;
}

AnnAudioEngine* AnnEngine::getAudioEngine()
{
	return AudioEngine;
}

Ogre::SceneManager* AnnEngine::getSceneManager()
{
	return m_SceneManager;
}

double AnnEngine::getTimeFromStartUp()
{
	return static_cast<double>(oor->getTimer()->getMilliseconds());//Why ?? O.O 
}

////////////////////////////////////////////////////////// SETTERS
void AnnEngine::setDebugPhysicState(bool state)
{
	assert(physicsEngine);
	log("Activating debug drawing for physics engine");
	physicsEngine->setDebugPhysics(state);
}

void AnnEngine::setAmbiantLight(Ogre::ColourValue v)
{
	std::stringstream ss; ss << "Setting the ambiant light to color " << v; log(ss.str());
	m_SceneManager->setAmbientLight(v);
}

void AnnEngine::setSkyDomeMaterial(bool activate, const char materialName[], float curvature, float tiling)
{
	log("Setting skydome from material"); log(materialName, false);
	m_SceneManager->setSkyDome(activate, materialName, curvature, tiling);
}

void AnnEngine::removeSkyDome()
{
	log("disabeling skydome");
	m_SceneManager->setSkyDomeEnabled(false);
}

void AnnEngine::setNearClippingDistance(Ogre::Real nearClippingDistance)
{
	if(oor)
		oor->setCamerasNearClippingDistance(nearClippingDistance);
}

void AnnEngine::resetPlayerPhysics()
{
	if(!player->hasPhysics()) return;
	log("player's physics is resseting");
	//Remove the player's rigidbody from the world
	physicsEngine->getWorld()->removeRigidBody(player->getBody());
	
	//We don't need that body anymore...
	delete player->getBody();
	//prevent memory access to unallocated address
	player->setBody(NULL);

	//Put everything back in order
	m_CameraReference->setPosition(player->getPosition());
	physicsEngine->createPlayerPhysicalVirtualBody(player, m_CameraReference);
	physicsEngine->addPlayerPhysicalBodyToDynamicsWorld(player);
}

OgrePose AnnEngine::getPoseFromOOR()
{
	if(oor)
		return oor->returnPose;
	OgrePose p; return p;
}
