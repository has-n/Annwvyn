/**
* \file AnnEngine.hpp
* \brief Main Annwvyn Engine class
*        handle intialization, destruction of object at runtime
*        handle rendering initialization, physics initialization and sound engine initialization
* \author A. Brainville (Ybalrid)
*/

#ifndef ANN_ENGINE
#define ANN_ENGINE

//Keep track of engine version here
#define ANN_MAJOR 0
#define ANN_MINOR 0
#define ANN_PATCH 12 
#define ANN_EXPERIMENTAL true

#include "systemMacro.h"

//C++ STD & STL
#include <cassert>
#include <list>

//Graphic rendering system for the rift
#include "OgreOculusRender.hpp"

//Annwvyn
#include "AnnEventManager.hpp"
#include "AnnTriggerObject.hpp"
#include "AnnTypes.h"
#include "AnnTools.h" 
#include "AnnAudioEngine.hpp"
#include "AnnPhysicsEngine.hpp"
#include "AnnConsole.hpp"
#include "AnnLevelManager.hpp"

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

namespace Annwvyn
{
	//For some reason, GCC don't want to compile that class without predeclaring AnnPhysicsEngine here.
	//even if the header of that class is included... 
	class AnnPhysicsEngine;

	///Main engine class. Creating an instance of that class make the engine start.
	///It's more or less a singleton, and will be the only one in the engine architecture. 
	///You can intantiate it like a normal class and bypass the idea of a singleton complettely.
	///This is the base class of the whole engine, the idea is more or less the one described in the 
	///"solutions to use a singleton for everything" in this article http://gameprogrammingpatterns.com/singleton.html

	class DLL AnnEngine
	{
	private:
		///the singleton address itself is stored here
		static AnnEngine* singleton;


	public:
		///This method is called by the OgreOculusRender object. Here is refresh code that needs to know current pose
		void toogleOculusPerfHUD();

		///Get the current instance of AnnEngine. pointer
		static AnnEngine* Instance();

		///Class constructor. take the name of the window
		/// \param title The title of the windows that will be created by the operating system
		/// \param fs the fullscreen state of the application. set it to false may help when developping with VS debugger on one screen
		AnnEngine(const char title[] = "Annwvyn Game Engine", bool fs = true);

		///Class destructor. Do clean up stuff.
		~AnnEngine();

		///Get the event manager
		AnnEventManager* getEventManager();

		///Get the player
		AnnPlayer* getPlayer();

		///////////////////////////////////////////////////////////////////////////////////////////////////////RESOURCE

		///Give a zipped archive resource location to the Ogre Resource Group Manager
		/// \param path The path to a zip file.
		/// \param resourceGroupName name of the resource group where the content will be added
		void loadZip(const char path[], const char resourceGroupName[] = "ANNWVYN_DEFAULT");

		///Give a directory resouce location to the Ogre Resource Group Manager
		/// \param path The path to the directory
		/// \param resourceGroupName name of the resource group
		void loadDir(const char path[], const char resourceGroupName[] = "ANNWVYN_DEFAULT");

		///Load a standard Ogre resource.cfg file
		/// \param path path to the resource file
		void loadResFile(const char path[]); //resource

		///Init All ressources groups
		void initResources(); //resource

		///Add to the default resource group "FileSystem=media" and "Zip=media/CORE.zip"
		void addDefaultResourceLocaton();

		///Init a resource group
		/// \param resourceGroup name of the resourceGroup
		void initAResourceGroup(std::string resourceGroup); //resource

		///////////////////////////////////////////////////////////////////////////////////////////////////////RESOURCE

		///Init OgreOculus stuff
		void oculusInit(); //oculus

		///Init the physics model
		void initPlayerPhysics(); //physics on player 

		///If the player is handeled throug the physics engine, this method will detach the rigidbody from the camera,
		///remove it from the dynamics world, unalocate it from the memory and recreate it from scratch. This is usefull for
		///"teleporting" the player, for example if you need to reset his position.
		void resetPlayerPhysics();

		///Create a game object form the name of an entity.
		/// \param entityName Name of an entity loaded to the Ogre ResourceGroupManager
		/// \param object An instance of an empty AnnGameObject. Usefull for creating object of herited class
		AnnGameObject* createGameObject(const char entityName[], AnnGameObject* object = new AnnGameObject); //object factory

		///Destroy the given object
		/// \param object the object to be destroyed
		bool destroyGameObject(AnnGameObject* object); //object factory
		
		//TODO remove destroy light
		void destroyLight(AnnLightObject* light){destroyLightObject(light);}
		///Destroy the given light
		/// \param light pointer to the light to destroy
		void destroyLightObject(AnnLightObject* light);

		///Set the ambiant light
		/// \param v the color of the light
		void setAmbiantLight(Ogre::ColourValue v); //scene parameter

		//TODO remove "addlight"
		AnnLightObject* addLight(){return createLightObject();}
		///Add a light source to the scene. return a pointer to the new light
		AnnLightObject* createLightObject();

		///Display bullet debuging drawing
		/// \param state debug state
		void setDebugPhysicState(bool state); //engine debug

		///Return true if the game want to terminate the program
		bool requestStop(); //engine

		///Log something to the console. If flag = true (by default), will print "Annwvyn - " in front of the message
		/// \param message Message to be loged 
		/// \param flag If true : Put the "Annwvyn -" flag before the message
		static void log(std::string message, bool flag = true); //engine

		///Refresh all for you
		bool refresh(); //engine main loop

		///Get elapsed time from engine startup
		double getTimeFromStartUp();//engine

		///Return the Annwvyn OpenAL simplified audio engine
		AnnAudioEngine* getAudioEngine(); //audio
		
		///Return the Physics Engine
		AnnPhysicsEngine* getPhysicsEngine();

		///Is key 'key' pressed ? (see OIS headers for KeyCode, generaly 'OIS::KC_X' where X is the key you want.
		/// key an OIS key code
		bool isKeyDown(OIS::KeyCode key); //event

		///Create a trigger object
		/// \param trigger an empty trigger object
		AnnTriggerObject* createTriggerObject(AnnTriggerObject* trigger = new AnnSphericalTriggerObject); //object factory

		///Get ogre scene manager
		Ogre::SceneManager* getSceneManager(); //scene or graphics

		///Set the ogre material for the skydome with params
		/// \param activate if true put a skydome
		/// \param materialName name of a material known from the Ogre Resource group manager
		/// \param curvature curvature of the texture
		/// \param tilling tilling of the texture
		void setSkyDomeMaterial(bool activate, 
			const char materialName[], 
			float curvature = 2.0f, 
			float tiling = 1.0f); //scene

		///Set the ogre material for the skybox with params
		/// \param activate if true put the skybox on the scene
		/// \param materialName name of a material declared on the resource manager
		/// \param distance distance of the sky from the camera
		/// \param renderedFirst if true, the skybox will be the first thing rendered
		void setSkyBoxMaterial(bool activate, 
			const char materialName[], 
			float distance = 8000, 
			bool renderedFirst = true);

		///Set the viewports background color
		/// \param v background color
		void setWorldBackgroudColor(Ogre::ColourValue v = Ogre::ColourValue(0, 0.56f, 1)); 

		///Remove the sky dome
		void removeSkyDome();
		void removeSkyBox();

		///Get the AnnObject the player is looking at
		AnnGameObject* playerLookingAt(); //physics

		///Get the AnnGameObject form the given Ogre node
		AnnGameObject* getFromNode(Ogre::SceneNode* node); //engine

		///Get ogre camera scene node
		Ogre::SceneNode* getCamera();

		///Reference orientation. Usefull if you are inside a vehicule for example
		/// \param q the reference orientation for the point of view. Usefull for applying vehicle movement to the player
		void setReferenceQuaternion(AnnQuaternion q); //engine...

		///Retrive the said reference quaternion
		AnnQuaternion getReferenceQuaternion(); //engine 

		///Attach a 3D mesh to the camera to act as player's body.
		/// \param entityName name of the entity that will serve as player body
		/// \param z_offset offset betwenn camera and player center eye pont
		/// \param flip if you need to flip the object to be correctly oriented (looking to negative Z)
		/// \param scale The scale to be aplied to the body object
		void attachVisualBody(const std::string entityName,  
			float z_offset = -0.0644f, 
			bool flip = false, 
			bool animated = false, 
			Ogre::Vector3 scale = Ogre::Vector3::UNIT_SCALE); //I seriously have something to do about that...

		///Reset the Rift Orientation
		void resetOculusOrientation();///Gameplay... but engine related function. 

		///Set the distance of the near clipping plane
		/// \param distace the distance to the clipping plane
		void setNearClippingDistance(Ogre::Real distance); //graphics

		///Set the engine to use the "default" event listener.
		///This will create an instance of AnnDefaultEventListener (if it doesn't allready exist inside of AnnEngine)
		///This will also unregister all listeners known by AnnEventListener
		///The default event listerner implement a simple "FPS-like" controll scheme 
		/// WASD for walking
		/// Horizontal view with mouse X relative mouvement
		/// That event listener is designed as an example of an event listener, and for exploring the environement without having to write a custom event listene
		void useDefaultEventListener();

		///Get the address of the default event listener declared by "use default event listener"
		AnnDefaultEventListener* getInEngineDefaultListener();

		///Get a pose information object
		OgrePose getPoseFromOOR();

		///Open a console and redirect standard output to it.
		///This is only effective on Windows. There is no other
		///simple way to acces the standard io on a Win32 application
		static void openConsole();

		///Get the current level manager
		AnnLevelManager* getLevelManager();

		///Toogle the display of the in-engine console
		void toogleOnScreenConsole();

		///Return a string descibing the version of the engine
		static std::string getAnnwvynVersion();

		///This start the reder loop. This also calls objects "atRefresh" and current level "runLogic" methods each frame
		void startGameplayLoop();

		///Remove the object from the engine
		void destroyTriggerObject(AnnTriggerObject* obj);

	private:
		///The onScreenConsole object
		static AnnConsole* onScreenConsole;

		//Audio engine
		AnnAudioEngine* AudioEngine;
		//Player
		AnnPlayer* player;
		//Event manager
		AnnEventManager* eventManager;
		//This event listener do WASD+mouse movement on the player
		AnnDefaultEventListener* defaultEventListener;
		//Physics
		AnnPhysicsEngine* physicsEngine;
		//LevelManager
		AnnLevelManager* levelManager;

		//The window created by OGRE that receive event for OIS
		Ogre::RenderWindow* m_Window;
		//The scene manager
		Ogre::SceneManager* m_SceneManager;
		//Where to put the camera
		Ogre::SceneNode* m_CameraReference;
		//Where the visualBody is attached
		Ogre::SceneNode* VisualBodyAnchor;
		//Orientation offcet between the model and the cameras
		AnnQuaternion refVisualBody;
		//The entity representing the player
		Ogre::Entity* VisualBody;
		//The animation state of the player
		Ogre::AnimationState* VisualBodyAnimation;
		//offset in Z axis of the visual body
		float visualBody_Zoffset;
		//Can load resources
		bool readyForLoadingRessources;
		//Oculus oculus;
		OgreOculusRender* renderer;

		///Dynamic container for games objects present in engine.
		std::list<AnnGameObject*>	objects;
		std::list<AnnTriggerObject*> triggers;
		std::list<AnnLightObject*> lights;

		///Elapsed time between 2 frames
		double deltaT; 
		double lastFrameTimeCode;
		double currentFrameTimeCode;
		bool fullscreen;
		bool lockForCallback;
		std::vector<AnnGameObject*> clearingQueue;
		std::vector<AnnTriggerObject*> triggerClearingQueue;
		void clearTriggers();
#ifdef __gnu_linux__
		std::string x11LayoutAtStartup;
#endif
	};
}
#endif ///ANN_ENGINE
