#ifndef ANN_LEVELMANAGER
#define ANN_LEVELMANAGER

//#include <Annwvyn.h>
#include <vector>
#include "AnnAbstractLevel.hpp"

namespace Annwvyn
{
	typedef size_t level_id;
	///Class that take care of switching between differents levels dynamicly and clearing the memory afterwards
	class DLL AnnLevelManager
	{
	public:
		///Construct LevelManager
		AnnLevelManager();
		///Destroy the level manager
		~AnnLevelManager();

		///Jump to an index referenced level
		///\param levelId Index of the level in the order they have been declraed
		void jump(level_id levelId);
		///Jump to a pointer referenced level
		///\param level address of a subclass instance of AbstractLevel
		void jump(AnnAbstractLevel* level);
		///Add a level to the level manager
		///\param level address of a subclass instance of AbstractLevel
		void addLevel(AnnAbstractLevel* level);
		///Jumpt to the 1st level
		void jumpToFirstLevel();
		///Run level logic
		void step();
		///Unload the level currently running
		void unloadCurrentLevel();

	private:
		///List of levels
		std::vector<AnnAbstractLevel*> levelList;
		///Address to the currently running level
		AnnAbstractLevel* current;
	};
}
#endif LEVELMANAGER