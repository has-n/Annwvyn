#include "AnnEventManager.hpp"
#include "AnnEngine.hpp"//to access logger static method
using namespace Annwvyn;

AnnAbstractEventListener::AnnAbstractEventListener(AnnPlayer* p)
{
	player = p;
}

AnnEvent::AnnEvent() :
	accepted(false),
	rejected(false),
	unpopulated(true),
	valid(false)
{
}

void AnnEvent::validate()
{
	valid = true;
}

void AnnEvent::populate()
{
	unpopulated = false;
}

AnnEventManager::AnnEventManager(Ogre::RenderWindow* w) :
	listener(NULL),
	Keyboard(NULL),
	Mouse(NULL),
	Joystick(NULL)
{

	for(size_t i(0); i < KeyCode::SIZE; i++) previousKeyStates[i] = false;

	//AnnEngine::log("Initialize OIS");
	InputManager = NULL;

	size_t windowHnd;
	std::stringstream windowHndStr;
	w->getCustomAttribute("WINDOW",&windowHnd);
	windowHndStr << windowHnd;

	pl.insert(std::make_pair(
		std::string("WINDOW"), windowHndStr.str()));

	InputManager = OIS::InputManager::createInputSystem(pl);

	Keyboard = static_cast<OIS::Keyboard*>(InputManager->createInputObject(OIS::OISKeyboard, true));
	Mouse = static_cast<OIS::Mouse*>(InputManager->createInputObject(OIS::OISMouse, true));

	if(InputManager->getNumberOfDevices(OIS::OISJoyStick) > 0)
	{
			Joystick = static_cast<OIS::JoyStick*>(InputManager->createInputObject(OIS::OISJoyStick, true));
			Annwvyn::AnnEngine::log(Joystick->vendor());
	}
}

AnnEventManager::~AnnEventManager()
{
	delete Keyboard;
	delete Mouse;
	delete Joystick;
	//delete InputManager;
}

void AnnEventManager::setListener(AnnAbstractEventListener* l)
{
	listener = l;
}

void AnnEventManager::removeListener()
{
	listener = NULL;
}

void AnnEventManager::update()
{
	//Capture events
	Keyboard->capture();
	Mouse->capture();
	if(Joystick)
		Joystick->capture();

	//if keyboard system initialized
	if(Keyboard)
	{
		//for each key of the keyboard
		for(size_t c (0); c < KeyCode::SIZE; c++)
		{
			//if it's pressed
			if(Keyboard->isKeyDown(static_cast<OIS::KeyCode>(c)))
			{
				//and wasn't before
				if(!previousKeyStates[c])
				{
					//create a coresponding key event 
					AnnKeyEvent e;
					e.setCode((KeyCode::code)c);
					e.setPressed();
					e.populate();
					e.validate();
					if(listener) //notify an eventual listener
						listener->KeyEvent(e);

					previousKeyStates[c] = true;
				}
			}
			else //key not pressed atm
			{
				//but was pressed just before
				if(previousKeyStates[c])
				{
					//same thing
					AnnKeyEvent e;
					e.setCode((KeyCode::code)c);
					e.setRelased();
					e.populate();
					e.validate();
					if(listener)
						listener->KeyEvent(e);

					previousKeyStates[c] = false;
				}
			}
		}
	}

	if(Mouse)
	{
		OIS::MouseState state(Mouse->getMouseState());
		AnnMouseEvent e;
		for(size_t i(0); i < MouseButtonId::nbButtons; i++)
			e.setButtonStatus(MouseButtonId(i),state.buttonDown(OIS::MouseButtonID(i)));

		e.setAxisInformation(MouseAxisId::X,AnnMouseAxis(MouseAxisId::X, state.X.rel, state.X.abs));
		e.setAxisInformation(MouseAxisId::Y,AnnMouseAxis(MouseAxisId::Y, state.Y.rel, state.Y.abs));
		e.setAxisInformation(MouseAxisId::Z,AnnMouseAxis(MouseAxisId::Z, state.Z.rel, state.Z.abs));

		e.populate();
		e.validate();

		if(listener)
			listener->MouseEvent(e);
	}

	if(Joystick)
	{
		//not implemented yet
	}
}