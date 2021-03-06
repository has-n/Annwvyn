#pragma once

#include "systemMacro.h"
#include "AnnSubsystem.hpp"
#include "AnnTypes.h"

#include <OgreMesh.h>
#include <OgreMesh2.h>

#include <memory>

namespace Annwvyn
{
	class AnnEngine;
	class AnnGameObject;

	///Game object manager. Create, destroy and keep track of objects, lights and other movable stuff
	class AnnDllExport AnnGameObjectManager : public AnnSubSystem
	{
	public:
		AnnGameObjectManager();

		///Update from the game engine
		void update() override;

		///Get a MeshPtr by loading a v1Mesh ptr, specify the name and where to put the 2 pointers
		Ogre::MeshPtr getAndConvertFromV1Mesh(const char* meshName, Ogre::v1::MeshPtr& v1Mesh, Ogre::MeshPtr& v2Mesh) const;

		///Create a game object form the name of an entity.
		/// \param mesh Name of an mesh loaded to the Ogre ResourceGroupManager
		/// \param object An instance of an empty AnnGameObject. Useful for creating object of inherited class
		std::shared_ptr<AnnGameObject> createGameObject(const std::string& mesh, std::string identifier = "",
														std::shared_ptr<AnnGameObject> object = std::make_shared<AnnGameObject>()); //object factory

		///Remove object from the manager. Object will be destroyed when no more references are in scope
		/// \param object the object to remove
		void removeGameObject(std::shared_ptr<AnnGameObject> object);

		///Search for an AnnGameObject that holds this node, returns it if found. Return nullptr if not found.
		std::shared_ptr<AnnGameObject> getFromNode(Ogre::SceneNode* node);

		///Remove the given light from the scene
		/// \param light shared pointer to the light
		void removeLightObject(std::shared_ptr<AnnLightObject> light);

		///Add a light source to the scene. return a pointer to the new light
		std::shared_ptr<AnnLightObject> createLightObject(std::string identifier = "");

		///Create a trigger object
		std::shared_ptr<AnnTriggerObject> createTriggerObject(std::string identifier = ""); //object factory

		///Remove the object from the engine
		void removeTriggerObject(std::shared_ptr<AnnTriggerObject> trigger);

		///Get the AnnGameObject the player is looking at
		std::shared_ptr<AnnGameObject> playerLookingAt(unsigned short limit = 5); //physics

		///Get an AnnGameObject for the required string; return nullptr if object cannot be found
		std::shared_ptr<AnnGameObject> getGameObject(std::string gameObjectName);

		///Get an AnnLightObject from it's name; return nullptr if object not found
		std::shared_ptr<AnnLightObject> getLightObject(std::string lightObjectName);

		///Get an AnnTriggerObject from it's name; return nullptr if object not found
		std::shared_ptr<AnnTriggerObject> getTriggerObject(std::string triggerObjectName);

		///Set the options to pass while converting Ogre V1 meshes to Ogre V2 meshes
		void setImportParameter(bool halfPosition, bool halfTextureCoord, bool qTangents);

	private:
		friend class AnnEngine;

		///Dynamic container for triggers objects present in engine.
		AnnTriggerObjectList Triggers;
		///Dynamic container for lights objects present in engine.
		AnnLightList Lights;
		///Dynamic container for Game objects present in engine
		AnnGameObjectList Objects;

		///objects mapped to ID strings
		std::unordered_map<std::string, std::shared_ptr<AnnGameObject>> identifiedObjects;

		///lights mapped to ID strings
		std::unordered_map<std::string, std::shared_ptr<AnnLightObject>> identifiedLights;

		///triggers identified to ID string
		std::unordered_map<std::string, std::shared_ptr<AnnTriggerObject>> identifiedTriggerObjects;

		uID autoID;
		uID nextID();

		bool halfPos, halfTexCoord, qTan;
	};

	using AnnGameObjectManagerPtr = std::shared_ptr<AnnGameObjectManager>;
}
