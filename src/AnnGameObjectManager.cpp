#include "stdafx.h"
#include "AnnGameObjectManager.hpp"
#include "AnnLogger.hpp"
#include "AnnEngine.hpp"
#include "AnnGetter.hpp"
#include "AnnException.hpp"

using namespace Annwvyn;
unsigned long long AnnGameObjectManager::id;

AnnGameObjectManager::AnnGameObjectManager() : AnnSubSystem("GameObjectManager")
{
	//There will only be one manager, set the id to 0
	id = 0;
}

void AnnGameObjectManager::update()
{
	//Run animations and update OpenAL sources position
	for (auto gameObject : Objects)
	{
		gameObject->addAnimationTime(AnnGetEngine()->getFrameTime());
		gameObject->updateOpenAlPos();
		gameObject->atRefresh();
		gameObject->callUpdateOnScripts();
	}
}

std::shared_ptr<AnnGameObject> AnnGameObjectManager::createGameObject(const char meshName[], std::string identifier, std::shared_ptr<AnnGameObject> obj)
{
	AnnDebug("Creating a game object from the mesh file :  " + std::string(meshName));
	auto smgr{ AnnGetEngine()->getSceneManager() };
	
	//auto ent = smgr->createEntity(meshName);

	//We are using Ogre v1 mesh format : 
	auto v1Mesh = Ogre::v1::MeshManager::getSingleton().load(meshName,
		Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,
		Ogre::v1::HardwareBuffer::HBU_STATIC,
		Ogre::v1::HardwareBuffer::HBU_STATIC);

	static const std::string sufix = "_V2mesh";

	//Generate the name of the v2 mesh 
	auto v2meshName = meshName + sufix;
	AnnDebug() << "Mesh v2 name : " << v2meshName;

	//See it the mesh allready exist :
	Ogre::MeshPtr v2Mesh = {};
	v2Mesh = Ogre::MeshManager::getSingleton().getByName(v2meshName);
	if (!v2Mesh) //create and import 
	{
		AnnDebug() << v2Mesh << " doesn't exist yet in the v2 MeshManager, creating it and loading the v1 " << meshName << " geometry";
		v2Mesh = Ogre::MeshManager::getSingleton().createManual(v2meshName, AnnResourceManager::defaultResourceGroupName);
		// TODO : permit to set theses things by hand
		v2Mesh->importV1(v1Mesh.get(), true, true, true);
	}

	//Create an item
	Ogre::Item* item = smgr->createItem(v2Mesh);
	
	//Create a node
	auto node = smgr->getRootSceneNode()->createChildSceneNode();
	
	//Attach
	node->attachObject(item);

	//Set GameObject members
	obj->setNode(node);
	obj->setItem(item);
	obj->setPhysicsMesh(v1Mesh);
	obj->audioSource = AnnGetAudioEngine()->createSource();

	//id will be unique to every non-identified object.
	//The identifier name can be empty, meaning that we have to figure out an unique name.
	//In that case we will append to the entity name + a number that will always be incremented.
	if (identifier.empty())
		identifier = meshName + std::to_string(++id);

	AnnDebug() << "The object " << identifier << " has been created. Annwvyn memory address " << obj;
	AnnDebug() << "This object take " << sizeof *obj.get() << " bytes";

	obj->name = identifier;
	identifiedObjects[identifier] = obj;
	Objects.push_back(obj);

	obj->postInit();
	return obj;
}

void AnnGameObjectManager::removeGameObject(std::shared_ptr<AnnGameObject> object)
{
	AnnDebug() << "Removed object " << object->getName();

	if (!object) throw AnnNullGameObjectError();

	Objects.remove(object);
	identifiedObjects.erase(object->getName());
}

std::shared_ptr<AnnGameObject> AnnGameObjectManager::getFromNode(Ogre::SceneNode* node)
{
	AnnDebug() << "Trying to identify object at address " << static_cast<void*>(node);

	auto result = std::find_if(Objects.begin(), Objects.end(),
		[&](std::shared_ptr<AnnGameObject> object) {return object->getNode() == node; });
	if (result != Objects.end())
		return *result;

	AnnDebug() << "The Scene Node" << static_cast<void*>(node) << " doesn't belong to any AnnGameObject";
	return nullptr;
}

void AnnGameObjectManager::removeLightObject(std::shared_ptr<AnnLightObject> light)
{
	Lights.remove(light);
}

std::shared_ptr<AnnLightObject> AnnGameObjectManager::createLightObject()
{
	AnnDebug("Creating a light");
	auto Light = std::make_shared<AnnLightObject>(AnnGetEngine()->getSceneManager()->createLight());
	Light->setType(AnnLightObject::LightTypes::ANN_LIGHT_POINT);
	Lights.push_back(Light);
	return Light;
}

std::shared_ptr<AnnTriggerObject> AnnGameObjectManager::createTriggerObject(std::shared_ptr<AnnTriggerObject> trigger)
{
	AnnDebug("Creating a trigger object");
	Triggers.push_back(trigger);
	trigger->postInit();
	return trigger;
}

void AnnGameObjectManager::removeTriggerObject(std::shared_ptr<AnnTriggerObject> trigger)
{
	Triggers.remove(trigger);
}

std::shared_ptr<AnnGameObject> AnnGameObjectManager::playerLookingAt(unsigned short limit)
{
	//Origin vector of the ray is the HMD pose position
	auto hmdPosition{ AnnVect3(AnnGetEngine()->getHmdPose().position) };

	//Calculate direction Vector of the ray to be the midpoint camera optical axis
	auto rayDirection{ AnnQuaternion(AnnGetEngine()->getHmdPose().orientation).getAtVector() };

	//create ray
	Ogre::Ray ray(hmdPosition, rayDirection);

	//create query
	auto raySceneQuery(AnnGetEngine()->getSceneManager()->createRayQuery(ray));
	//Sort by distance. Nearest is first. Limit to `limit` results.
	raySceneQuery->setSortByDistance(true, limit);

	//execute and get the results
	auto& results(raySceneQuery->execute());

	//read the result list
	auto result = std::find_if(results.begin(), results.end(), [](const Ogre::RaySceneQueryResultEntry& entry)
	{
		if (entry.movable && entry.movable->getMovableType() == "Entity") return true;
		return false;
	});

	//If can't find it? return nullptr
	if (result == results.end())
		return nullptr; //means that we don't know what the player is looking at.

	//We got access to the node, we want the object
	return getFromNode(result->movable->getParentSceneNode());
}

std::shared_ptr<AnnGameObject> AnnGameObjectManager::getObjectFromID(std::string idString)
{
	auto object = identifiedObjects.find(idString);
	if (object != identifiedObjects.end())
		return object->second;
	return nullptr;
}