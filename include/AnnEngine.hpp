/**
* \file AnnEngine.hpp
* \brief Main Annwvyn Engine class
*        Initialize the renderer and all subsystem. Provide access to all of the engine components
* \author A. Brainville (Ybalrid)
*/

#ifndef ANN_ENGINE
#define ANN_ENGINE

//Keep track of engine version here
#define ANN_MAJOR 0
#define ANN_MINOR 3
#define ANN_PATCH 2
#define ANN_EXPERIMENTAL true

#include "systemMacro.h"

//C++ STD & STL
#include <cassert>
#include <list>
#include <memory>

//Graphic rendering system for the rift
#include "AnnOgreOculusRenderer.hpp"

//Graphic rendering system for the vive
#include "AnnOgreOpenVRRenderer.hpp"

//Graphic rendering system for NO FREAKING VR SYSTEM
#include "AnnOgreNoVRRenderer.hpp"

//Annwvyn
#include "AnnEventManager.hpp"
#include "AnnPhysicsEngine.hpp"
#include "AnnTriggerObject.hpp"
#include "AnnTypes.h"
#include "AnnAudioEngine.hpp"
#include "AnnConsole.hpp"
#include "AnnLevelManager.hpp"
#include "AnnFilesystem.hpp"
#include "AnnResourceManager.hpp"
#include "AnnGameObject.hpp"
#include "AnnGameObjectManager.hpp"
#include "AnnSceneryManager.hpp"
#include "AnnScriptManager.hpp"
#include "AnnStringUtility.hpp"

#include "AnnUserSpaceSubSystem.hpp"

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

//Get the deprecated warnings
#pragma warning(default:4996)

namespace Annwvyn
{
	class AnnEngine;

	class AnnPhysicsEngine;

	///Utility class for AnnEngine
	class AnnDllExport AnnEngineSingletonReseter
	{
	private:
		AnnEngineSingletonReseter(AnnEngine* address);
		~AnnEngineSingletonReseter();
		friend class AnnEngine;
		AnnEngine* engine;
	};

	///Main engine class. Creating an instance of this class make the engine start.
	class AnnDllExport AnnEngine
	{
		//It's more or less a singleton, and will be the only one in the engine
		//architecture. You can instantiate it like a normal class and bypass the
		//idea of a singleton completely. This is the base class of the whole
		//engine, the idea is more or less the one described in the "solutions to
		//use a singleton for everything" in this article
		//http://gameprogrammingpatterns.com/singleton.html

		///the singleton address itself is stored here
		static AnnEngine* singleton;
		friend class AnnEngineSingletonReseter;
		AnnEngineSingletonReseter resetGuard;

		void selectAndCreateRenderer(const std::string& hmd, const std::string& title);

	public:

		static bool autosetProcessPriorityHigh;
		static void setProcessPriorityNormal();
		static void setProcessPriorityHigh();
		static bool noConsoleColor;
		static bool manualConsole;

		static void setNoConsoleColor();

		///Get the current instance of AnnEngine. pointer
		static AnnEngine* Instance();

		///Return a string describing the version of the engine
		static std::string getAnnwvynVersion(long long int padding = -1);

		///Class constructor. take the name of the window
		/// \param title The title of the windows that will be created by the operating system
		AnnEngine(const char title[], std::string hmdCommand);

		///Class destructor. Do clean up stuff.
		~AnnEngine();

		///Log something to the console. If flag = true (by default), will print "Annwvyn - " in front of the message
		/// \param message Message to be logged
		/// \param flag If true : Put the "Annwvyn -" flag before the message
		static void log(std::string message, bool flag = true); //engine

		///Get the player
		AnnPlayerBodyPtr getPlayer() const;

		///Is key 'key' pressed ? (see OIS headers for KeyCode, generally 'OIS::KC_X' where X is the key you want.
		/// key an OIS key code
		bool isKeyDown(OIS::KeyCode key) const; //event

		///Get ogre camera scene node
		Ogre::SceneNode* getPlayerPovNode() const;

		///Open a console and redirect standard output to it.
		///This is only effective on Windows. There is no other
		///simple way to access the standard io on a Win32 application
		static bool openConsole();

		///Get ogre scene manager
		Ogre::SceneManager* getSceneManager() const; //scene or graphics

		/////////////////////////////////////////////////////////////////////////////////////////////////////SUBSYSTEMS

		///Get the event manager
		AnnEventManagerPtr getEventManager() const;

		///Get the file-system manager
		AnnFilesystemManagerPtr getFileSystemManager() const;

		///Return the Annwvyn OpenAL simplified audio engine
		AnnAudioEnginePtr getAudioEngine() const; //audio

		///Return the Physics Engine
		AnnPhysicsEnginePtr getPhysicsEngine() const;

		///Get the current level manager
		AnnLevelManagerPtr getLevelManager() const;

		///Get the ResourceManager
		AnnResourceManagerPtr getResourceManager() const;

		///Get the GameObjectManager
		AnnGameObjectManagerPtr getGameObjectManager() const;

		///Get the SceneryManager
		AnnSceneryManagerPtr getSceneryManager() const;

		///Get the ScriptManager
		AnnScriptManagerPtr getScriptManager() const;

		///Get the VRRenderer
		AnnOgreVRRendererPtr getVRRenderer() const;

		///Get the console
		AnnConsolePtr getOnScreenConsole() const;

		///
		AnnStringUtilityPtr getStringUtility() const;

		/////////////////////////////////////////////explicit /////////////////////////////////////////////////END OF SUBSYSTEMS

		///Init the static/standing physics model
		void initPlayerStandingPhysics() const;

		///Init the roomscale physics model
		void initPlayerRoomscalePhysics() const;

		///Return true if the game want to terminate the program
		bool requestStop() const; //engine

		///Refresh all for you
		bool refresh(); //engine main loop

		///Set the POV node to the AnnPlayerBody gameplay defined position/orientation of the player's body
		void syncPov() const;

		///This start the render loop. This also calls objects "atRefresh" and current level "runLogic" methods each frame
		void startGameplayLoop();

		///Return true if the app is visible inside the head mounted display
		bool appVisibleInHMD() const;

		///Get elapsed time from engine startup in millisecond
		unsigned long getTimeFromStartUp() const;//engine

		///Get elapsed time from engine startup in seconds
		double getTimeFromStartupSeconds() const;

		///Get elapsed time between two frames in seconds
		double getFrameTime() const;

		///Get the pose of the HMD in VR world space
		AnnPose getHmdPose() const;

		///Register your own subsystem to be updated by the engine
		std::shared_ptr<AnnUserSubSystem> registerUserSubSystem(std::shared_ptr<AnnUserSubSystem> userSystem);

		///Get pointer to a subsystem by name
		std::shared_ptr<AnnSubSystem> getSubSystemByName(std::string name);

		///Know if subsystem is user defined
		static bool isUserSubSystem(std::shared_ptr<AnnSubSystem> subsystem);

		///Remove a subsystem form the engine. Only works if the system has been user defined.
		void removeUserSubSystem(std::shared_ptr<AnnUserSubSystem> subsystem);

		///Permit
		void requestQuit();

	private:

#ifdef _WIN32
		static WORD consoleGreen;
		static WORD consoleYellow;
		static WORD consoleWhite;
#endif
		static bool consoleReady;
		bool applicationQuitRequested;

		///String Utility;
		AnnStringUtilityPtr stringUtility;
		///VR renderer
		AnnOgreVRRendererPtr renderer;
		///The onScreenConsole object
		AnnConsolePtr onScreenConsole;
		///ResourceManager
		AnnResourceManagerPtr resourceManager;
		///SceneryManager
		AnnSceneryManagerPtr sceneryManager;
		///FilesystemManager
		AnnFilesystemManagerPtr filesystemManager;
		///Audio engine
		AnnAudioEnginePtr audioEngine;
		///Event manager
		AnnEventManagerPtr eventManager;
		///Physics
		AnnPhysicsEnginePtr physicsEngine;
		///GameObjectManager
		AnnGameObjectManagerPtr gameObjectManager;
		///LevelManager
		AnnLevelManagerPtr levelManager;
		///ScriptManager
		AnnScriptManagerPtr scriptManager;
		///Player
		AnnPlayerBodyPtr player;

		///The scene manager
		Ogre::SceneManager* SceneManager;
		///Point Of View : Node used as "root" for putting the VR "camera rig"
		Ogre::SceneNode* vrRendererPovGameplayPlacement;

		///Elapsed time between 2 frames
		double updateTime;

		///Container for all the subsystem. Populated in the update/delete order
		std::list<std::shared_ptr<AnnSubSystem>> SubSystemList;
	};
}
#endif ///ANN_ENGINE