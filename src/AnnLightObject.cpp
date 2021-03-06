// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"
#include "AnnLightObject.hpp"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"
#include "AnnGetter.hpp"

using namespace Annwvyn;

AnnLightObject::AnnLightObject(Ogre::Light* light, const std::string& name) :
 light(light),
 name(name)
{
	AnnDebug() << "Light constructor called";
	if(light)
	{
		node = AnnGetEngine()->getSceneManager()->getRootSceneNode()->createChildSceneNode();
		node->attachObject(light);
	}
}

AnnLightObject::~AnnLightObject()
{
	AnnDebug() << "Light destructor called";
	if(light)
	{
		if(node)
		{
			node->detachObject(light);
			if(node->getParentSceneNode())
				node->getParentSceneNode()->removeChild(node);
			AnnGetEngine()->getSceneManager()->destroySceneNode(node);
			node = nullptr;
		}
		AnnGetEngine()->getSceneManager()->destroyLight(light);
	}
}

AnnLightObject::LightTypes AnnLightObject::getLightTypeFromString(const std::string& ltype)
{
	if(ltype == "Point") return ANN_LIGHT_POINT;
	if(ltype == "Spotlight") return ANN_LIGHT_SPOTLIGHT;
	if(ltype == "Directional") return ANN_LIGHT_DIRECTIONAL;
	return ANN_LIGHT_POINT;
}

void AnnLightObject::setPosition(AnnVect3 position)
{
	node->setPosition(position);
}

void AnnLightObject::setDirection(AnnVect3 direction) const
{
	light->setDirection(direction);
}

AnnVect3 AnnLightObject::getDirection() const
{
	return light->getDirection();
}

void AnnLightObject::setType(LightTypes type) const
{
	light->setType(Ogre::Light::LightTypes(type));
}

void AnnLightObject::setDiffuseColor(AnnColor color) const
{
	light->setDiffuseColour(color.getOgreColor());
}

void AnnLightObject::setSpecularColor(AnnColor color) const
{
	light->setSpecularColour(color.getOgreColor());
}

AnnColor AnnLightObject::getDiffuseColor() const
{
	return AnnColor(light->getDiffuseColour());
}

AnnColor AnnLightObject::getSpecularColor() const
{
	return AnnColor(light->getSpecularColour());
}

AnnVect3 AnnLightObject::getPosition()
{
	return node->getPosition();
}

void AnnLightObject::setAttenuation(float range, float constant, float linear, float quadratic) const
{
	light->setAttenuation(range, constant, linear, quadratic);
}

Ogre::Light* AnnLightObject::_getOgreLight() const
{
	return light;
}

std::string AnnLightObject::getName() const
{
	return name;
}

void AnnLightObject::setPower(float lumens) const
{
	light->setPowerScale(lumens);
}

float AnnLightObject::getPower() const
{
	return light->getPowerScale();
}
