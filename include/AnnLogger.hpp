#ifndef ANN_LOGGER
#define ANN_LOGGER

#include "systemMacro.h"
//The debug output is opened by the AnnEngine class
#include "AnnEngine.hpp"
//We need the standard string fromat to be accesible
#include <string>
#include <iostream>
namespace Annwvyn
{
	class DLL AnnDebug : public std::ostream
	{
	private:
		class AnnDebugBuff : public std::stringbuf
		{
		public:
			///Construct an AnnDebug buffer
			AnnDebugBuff(){};
			///Will sync the buffer
			~AnnDebugBuff(){pubsync();};
			///Sync the buffer by performing an AnnEngine::log, clear it and return success. 
			int sync(){AnnEngine::log(str()); str(""); return 0;};
		};

	public:
		///Create an AnnDebug object that offer you a output stream to the AnnEngine logger
		///This permit you to write messages to the log using C++ style ostream
		/// example : AnnDebug() << "Player life is now " << playerLife;
		/// where playerLife is a variable. Everything that works with a std::ostream works here.
		AnnDebug();
		///Permit to log a static string via the debug stream
		/// \copydoc AnnEngine::AnnDebug()
		AnnDebug(const std::string& message);
		~AnnDebug();
	};
}

#endif