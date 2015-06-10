#include "stdafx.h"
#include "AnnAbstractLevel.hpp"
#include "AnnEngine.hpp"

using namespace Annwvyn;

AnnAbstractLevel::AnnAbstractLevel()
{
	AnnEngine::Instance()->log("A level has been created");
}

AnnAbstractLevel::~AnnAbstractLevel()
{
	unload();
	AnnEngine::Instance()->log("Destroying a level");
}

void AnnAbstractLevel::unload()
{
	//Remove background music
	AnnEngine::Instance()->getAudioEngine()->stopBGM();
	//Remove the sky
	AnnEngine::Instance()->removeSkyDome();

	//Remove the ambiant lighting
	AnnEngine::Instance()->setAmbiantLight(Ogre::ColourValue::Black);
	
	//Remove the level lights
	for(AnnLightVect::iterator it = levelLighting.begin(); it != levelLighting.end(); ++it)
		AnnEngine::Instance()->getSceneManager()->destroyLight(*it);
	levelLighting.clear();

	//Remove the level objects
	for(AnnGameObjectVect::iterator it = levelContent.begin(); it != levelContent.end(); ++it)
		AnnEngine::Instance()->destroyGameObject(*it);
	levelContent.clear();
}