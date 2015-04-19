/**
* * \file main.cpp
* \brief test/demo program
* \author Arthur Brainville 
* \version 0.5
* \date 06/11/14
*
* Annwvyn test program http://annwvyn.org/
*
*/

//C++ includes
#include <iostream>
#include <streambuf>
#include <fstream>
//Annwvyn
#include <Annwvyn.h>

#include <Gorilla.h>

using namespace std;

class Sinbad : public Annwvyn::AnnGameObject
{
public:
	void postInit()
	{
		setPos(0,2,3);
		setScale(0.2,0.2,0.2);
		setAnimation("Dance");
		playAnimation(true);
		loopAnimation(true);
	}
};


class MyTrigger : public Annwvyn::AnnTriggerObject
{
	void postInit()
	{
		//Radius in metter
		setThreshold(2);
	}

	void atContact()
	{
		//Print "Contact" to standard error stream
		std::cerr << "Contact" << std::endl;
	}
};

AnnMain()
{
	//create Annwvyn engine
	Annwvyn::AnnEngine* GameEngine = new Annwvyn::AnnEngine("A Game");	
	//CustomEventListener* el = new CustomEventListener(GameEngine->getPlayer());
	//GameEngine->getEventManager()->setListener(el);


	//load ressources
	//GameEngine->loadZip("media/CORE.zip");
	//GameEngine->loadDir("media");

	GameEngine->loadDir("media/dome");
	GameEngine->loadZip("media/Sinbad.zip");
	GameEngine->loadDir("media/plane");
	GameEngine->loadDir("media/body");
	GameEngine->loadDir("media/GUI");
	GameEngine->initResources();


	//Create Object
	
	MyTrigger* T = (MyTrigger*) GameEngine->createTriggerObject(new MyTrigger);
	T->setPosition(-0.5,-1,3);

	Annwvyn::AnnGameObject* Grid = GameEngine->createGameObject("Plane.mesh");
	Grid->setPos(0,-3,0);
	Grid->setUpBullet();
	GameEngine->setGround(Grid); 

	//Add light
	Annwvyn::AnnLightObject* Light = GameEngine->addLight();	
	GameEngine->setAmbiantLight(Ogre::ColourValue(.1f,.1f,.1f));
	Light->setPosition(0,3,10);


	GameEngine->initPlayerPhysics();
	GameEngine->setDebugPhysicState(false);


	//setUp Oculus system
	GameEngine->oculusInit();

	GameEngine->setSkyDomeMaterial(true,"Sky/dome1");


	//play background music
	//GameEngine->getAudioEngine()->playBGM("media/bgm/Blown_Away.ogg",0.2f); //volume 20%

	//Parameters : name of resource loaded "mesh" file, Z axis offset between center and eyball center, and a boolean for aplying a 180° flip or not
	//You have to flip the character if you modeled it whith the head facing you on your 3D software (witch is the good way to do it anyway...)

	GameEngine->attachVisualBody("male_Body.mesh",-0.1 ,true);
	
	//////////////////////////// INIT GUI TEST 
	Ogre::SceneNode* camera = GameEngine->getCamera();
	Ogre::SceneNode* GUI_root = camera->createChildSceneNode();
	GUI_root->translate(-0.6,0,-2);
	Gorilla::Silverback* svbk = new Gorilla::Silverback();
	svbk->loadAtlas("dejavu");

	Gorilla::ScreenRenderable* screen = svbk->createScreenRenderable(Ogre::Vector2(2.0f,1.0f),"dejavu");
	GUI_root->attachObject(screen);

	Gorilla::Layer* baseLayer = screen->createLayer(0);
	Gorilla::Rectangle* background = baseLayer->createRectangle(0,0,300,200);
	background->background_colour(Gorilla::rgb(255,0,0,0));

	Gorilla::Caption* testText = baseLayer->createCaption(9,20,20,"Test");
	testText->colour(Ogre::ColourValue::Black);
	testText->height(5);
	testText->_redraw();
	

	GameEngine->setNearClippingDistance(0.20f);

	GameEngine->resetOculusOrientation();

	AnnGameObject* S = GameEngine->createGameObject("Sinbad.mesh", new Sinbad);
	S->setPos(0,5,-5);
	S->setUpBullet(40, Annwvyn::boxShape, true);

    GameEngine->useDefaultEventListener();

	bool debounce;
	bool current(false);

	do	
	{   
		debounce = current;
		current = GameEngine->isKeyDown(OIS::KC_RETURN);

		if(current && !debounce)
			S->playSound("media/monster.wav");
		if(GameEngine->isKeyDown(OIS::KC_F12))
			GameEngine->resetOculusOrientation();	
	}while(GameEngine->refresh());

	delete GameEngine;
	return 0;
}
