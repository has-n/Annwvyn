#ifndef ANN_AUDIO
#define ANN_AUDIO
/*
#undef DLL
///windows DLL
#ifdef DLLDIR_EX
   #define DLL  __declspec(dllexport)   /// export DLL information
#else
   #define DLL  __declspec(dllimport)   /// import DLL information
#endif

///bypass on linux
#ifdef __gnu_linux__
#undef DLL
#define DLL
#endif
*/

#include "systemMacro.h"

#include <Ogre.h>

#include <iostream>
#include <string.h>

///OpenAl
#include <al.h>
#include <alc.h>
///libsndfile
#include <sndfile.h>


namespace Annwvyn
{
    class AnnEngine;
	class DLL AnnAudioEngine
	{
	public:
		///class constuctor
		AnnAudioEngine();
		///class destructor
		~AnnAudioEngine();
		///init openal
		
		bool initOpenAL();
		///shutdown and cleanup openal
		void shutdownOpenAL();
		
		///load a sound file. return a sond buffer
		ALuint loadSndFile(const std::string& Filename);
		
		
		///play background music. you can specify the volume of the music (0.0f to 1.0f)
		void playBGM(const std::string path, const float volume = 0.5f);

	private:
		///For engine : uptade listenter pos and orientation with Ogre coordinates
		void updateListenerPos(Ogre::Vector3 pos);
		
        ///For engine : update listener Oirentation
        
        friend class Annwvyn::AnnEngine;
    private:
        void updateListenerOrient(Ogre::Quaternion orient);
		std::string lastError;
		ALCdevice* Device;
		ALCcontext* Context;
	    
        ALuint buffer;
		ALuint bgm;///background music source
		///ALuint soundSource;
	};
}
#endif
