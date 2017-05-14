/**
* \file AnnEventManager.hpp
* \brief event management for Annwvyn
* \author A. Brainville (Ybalrid)
*/

#ifndef ANNEVENTMANAGER
#define ANNEVENTMANAGER

#include "systemMacro.h"

#include <array>
#include <memory>
#include <valarray>

#include "AnnKeyCode.h"
#include "AnnPlayer.hpp"
#include "AnnTriggerObject.hpp"
#include "AnnSubsystem.hpp"
#include "AnnEvents.hpp"
#include "AnnUserSpaceSubSystem.hpp"

//the following two macros exist only for my "please, look nicer" side
///Macro for declaring a listener
#define LISTENER public Annwvyn::AnnEventListener
///Macro for declaring a listener constructor
#define constructListener() AnnEventListener()

namespace Annwvyn
{
	class AnnEngine;
	class AnnDefaultEventListener;

	//The event manager handles all events that can occur during the gameplay loop. The private 'update()' method is called by
	//AnnEngine and provide the heartbeat for the event system.
	//Events can be user inputs or mostly anything else.
	//AnnEventManager creates AnnEvent (or subclass of AnnEvent) for each kind of event, populate that object with relevant event data
	//And propagate that event to any declared event listener.
	//Listeners should subclass AnnEventListener. A listener is registered when a pointer to it is passed as argument to the addListener() method.
	//You'll crash the engine if you destroy a listener without removing it from the EventManager (the EM will dereference an non-existing pointer)

	///Event Manager : Object that handle the event system
	class DLL AnnEventManager : public AnnSubSystem
	{
	public:
		///Construct the event manager
		AnnEventManager(Ogre::RenderWindow* w);
		///Destroy the event manager
		~AnnEventManager();

		//---------------------------- listener management
		///Set the engine to use the "default" event listener.
		///This will create an instance of AnnDefaultEventListener (if it doesn't already exist inside of AnnEngine)
		///This will also unregister all listeners known by AnnEventListener
		///The default event listener implement a simple "FPS-like" control scheme
		/// WASD for walking
		/// Horizontal view with mouse X relative movement
		/// That event listener is designed as an example of an event listener, and for exploring the environment without having to write a custom event listener
		void useDefaultEventListener();
		///Return the default event listener
		std::shared_ptr<AnnEventListener> getDefaultEventListener() const;
		///Ad a listener to the event manager
		/// \param listener Pointer to a listener object
		void addListener(std::shared_ptr<AnnEventListener> listener);
		///Remove every listener known from the EventManager.
		///This doesn't clear any memory
		void clearListenerList();
		///Make the event manager forget about the listener
		/// \param listener A listener object. If NULL (default), it will remove every listener form the manager (see clearListenerList())
		void removeListener(std::shared_ptr<AnnEventListener> listener = nullptr);
		//---------------------------- listener management

		//---------------------------- timer management
		///Create a timer that will timeout after "delay" seconds
		timerID fireTimer(double delay);
		///Create a timer that will timeout after "delay" milliseconds
		timerID fireTimerMillisec(double millisecDelay);
		//---------------------------- timer management

		//---------------------------- other
		///Get the number of available sticks
		size_t getNbStick() const;
		///Get the text inputer object
		AnnTextInputer* getTextInputer() const;
		///set the "shouldIgnore" flag to keyboard event
		void keyboardUsedForText(bool state = true);
		//---------------------------- other

	private:

		///List of pointer to the listeners.
		///The use of weak pointers permit to keep access to the listeners without having to own them.
		///This permit to use any classes of the engine (like levels) to be themselves event listener.
		std::vector<std::weak_ptr<AnnEventListener>> listeners;

		friend class AnnEngine;
		friend class AnnPhysicsEngine;
		friend class AnnUserSpaceEventLauncher;

		///Send the given event to the listeners
		void userSpaceDispatchEvent(std::shared_ptr<AnnUserSpaceEvent> e, AnnUserSpaceEventLauncher* sender);
		///Engine call for refreshing the event system
		void update() override;
		///Capture the event from OIS
		void captureEvents();
		///Process keyboard events
		void processKeyboardEvents();
		///Process mouse events
		void processMouseEvents();
		///Process joystick events
		void processJoystickEvents();
		///Process hand controller events
		void processHandControllerEvents();
		///Set the content of the event buffers to all registered listeners
		void pushEventsToListeners();
		///Process user inputs
		void processInput();
		///Process timers
		void processTimers();
		///Process triggers
		void processTriggerEvents();
		///Process collisions
		void processCollisionEvents();
		///Process user event dispatch()
		void processUserSpaceEvents();
		///Hook for the physics engine to signal collisions
		void detectedCollision(void* a, void* b);
		///Hook for the physics engine to signal player collision
		void playerCollision(void* object);

		///Buffer of keyboard events
		std::vector<AnnKeyEvent> keyEventBuffer;
		///Buffer of mouse events
		std::vector<AnnMouseEvent> mouseEventBuffer;
		///Buffer of stick events
		std::vector<AnnStickEvent> stickEventBuffer;
		///Buffer of hand controller events
		std::vector<AnnHandControllerEvent> handControllerEventBuffer;

		//----------------------- OIS and other library input objects
		///OIS Event Manager
		OIS::InputManager *InputManager;
		///Pointer that holds the keyboard
		OIS::Keyboard* Keyboard;
		///Pointer that holds the Mouse
		OIS::Mouse* Mouse;
		///parameters for OIS
		OIS::ParamList pl;
		///parameter list for OIS
		std::vector<JoystickBuffer*> Joysticks;
		//----------------------- OIS and other library input objects

		//----------------------- PREVIOUS STATE FOR EVENT DETECTION FROM UNBUFFERED STATE
		///Array for remembering the key states at last update.
		std::array<bool, KeyCode::SIZE> previousKeyStates;
		///Array for remembering the button states at last update
		std::array<bool, nbButtons> previousMouseButtonStates;
		//----------------------- PREVIOUS STATE FOR EVENT DETECTION FROM UNBUFFERED STATE

		//----------------------- TIMER MANAGEMENT
		///Dynamically sized array for remembering the joystick button state at last update
		timerID lastTimerCreated;
		///List of timers
		std::vector<AnnTimer> activeTimers;
		///List of timer that will timeout in a future frame
		std::vector<AnnTimer> futureTimers;
		///List of trigger event to process
		std::vector<AnnTriggerEvent> triggerEventBuffer;
		//----------------------- TIMER MANAGEMENT

		//----------------------- COLLISION MANAGEMENT
		///Collision reported by the physics engine to consider
		std::vector<std::pair<void*, void*>> collisionBuffer;
		///Player collision reported by the physics engine to consider
		std::vector<AnnGameObject*> playerCollisionBuffer;
		//----------------------- COLLISION MANAGEMENT

		///The text inputer object itself
		std::unique_ptr<AnnTextInputer> textInputer;
		///Default event listener
		std::shared_ptr<AnnDefaultEventListener> defaultEventListener;
		///Using a shared ptr to keep ownership of the event object until the event is dealt with. Also, polymorphism.
		std::vector<std::pair<std::shared_ptr<AnnUserSpaceEvent>, AnnUserSpaceEventLauncher*>> userSpaceEventBuffer;
		///ID of an eventual Xbox controller
		StickAxisId xboxID;

		///True if we detected an xbox controller
		bool knowXbox;
		///True if keyboard event should be ignored (keyboard used for "text input")
		bool keyboardIgnore;
	};
}

#endif //ANNEVENTMANAGER