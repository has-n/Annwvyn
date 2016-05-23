/**
* \file AnnLevel.hpp
* \brief Pure virtual class that represent a level
*        AnnLevelManager class only handle object that derive from this class
You need to overload at least the load() and runLogic() methods to create a level.
To simplify level declaration, you can use the keyword "LEVEL" to mark inheritance and
constructLevel() to call the constructor of the abstract class.
This class also provide a 'simple, stupid' random string generator to set IDs to object automaticaly
* \author A. Brainville (Ybalrid)
*/

#ifndef ANN_LEVEL
#define ANN_LEVEL

#include <unordered_map>

#include "AnnTypes.h"
#include "AnnTriggerObject.hpp"
#define LEVEL public Annwvyn::AnnLevel
#define constructLevel() AnnLevel()
#define noID "noID"
#define defaultIdLen 15

namespace Annwvyn
{
	///Base class for all Levels. Is absrtract
	class DLL AnnLevel
	{
	public:
		///Construct the level 
		AnnLevel();

		///Pure virtual methods that loads the level
		virtual void load() = 0;

		///Destroy the level
		virtual ~AnnLevel();

		///Unload the level by destroying every objects in "levelContent" and every lights in "levelLighting"
		virtual void unload();

		///Run logic code from the level
		virtual void runLogic() =0;

	protected:
		AnnGameObjectList levelContent;
		AnnLightList levelLighting;
		AnnTriggerObjectList levelTrigger;
		std::unordered_map<std::string, AnnGameObject*> levelContentIdMap;
		std::unordered_map<std::string, AnnLightObject*> levelLightingIdMap;
		std::unordered_map<std::string, AnnTriggerObject*> levelTriggerIdMap;

		///Add a light object to the level
		AnnLightObject* addLightObject(std::string id = noID);

		///Add a trigger object to the level
		AnnTriggerObject* addTrggerObject(AnnTriggerObject* obj = new AnnSphericalTriggerObject, std::string id = noID);

		///Add a Game object to the level
		AnnGameObject* addGameObject(std::string entityName, std::string id = noID);

		///Name of the level
		std::string name;

		///Create a random ID for level objects.
		std::string generateRandomID(size_t len = defaultIdLen);
	};
}
#endif