/**
* \file AnnLightObject.hpp
* \brief Object that represent a light source
* \author A. Brainville (Ybalrid)
*/
#ifndef ANN_LIGHT_OBJECT
#define ANN_LIGHT_OBJECT

#include "systemMacro.h"
#include <OgreLight.h>
#include "AnnVect3.hpp"
#include "AnnColor.hpp"

namespace Annwvyn
{
	class AnnEngine;


	///Light Object : Represent a light source
	class DLL AnnLightObject 
	{
	public:

		/// Defines the type of light
		enum LightTypes
		{
			/// Point light sources give off light equally in all directions, so require only position not direction
			ANN_LIGHT_POINT = Ogre::Light::LightTypes::LT_POINT,
			/// Directional lights simulate parallel light beams from a distant source, hence have direction but no position
			ANN_LIGHT_DIRECTIONAL = Ogre::Light::LightTypes::LT_DIRECTIONAL,
			/// Spotlights simulate a cone of light from a source so require position and direction, plus extra values for falloff
			ANN_LIGHT_SPOTLIGHT = Ogre::Light::LightTypes::LT_SPOTLIGHT
		};


		///Set the position of the light (if relevent)
		void setPosition(AnnVect3 position);
		///Set the direction of the light (if relevent)
		void setDirection(AnnVect3 direction);
		///Set the type of the light
		void setType(LightTypes type);
		///Set the diffuse color of this light source
		void setDiffuseColor(AnnColor color);
		///Set the specular color of this light source
		void setSpecularColor(AnnColor color);
		///Get the diffuse color of this light source
		AnnColor getDiffuseColor();
		///Get the specular color of this light source
		AnnColor getSpecularColor();

	private:
		///Create a light object. We use an Ogre Light becaus we just need to talk to Ogre...
		AnnLightObject(Ogre::Light* light);
		friend class AnnEngine;
		friend class AnnGameObjectManager;
		Ogre::Light* light;
	};
}

#endif