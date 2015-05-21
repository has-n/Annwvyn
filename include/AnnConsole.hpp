#ifndef	ANNCONSOLE
#define ANNCONSOLE

#include "systemMacro.h"
#include "AnnTypes.h"
#include <string>
#include <iostream>
#include <Ogre.h>
#include <Overlay\OgreFont.h>
#include <Overlay\OgreFontManager.h>

#define CONSOLE_BUFFER 20

namespace Annwvyn
{
	class DLL AnnConsole
	{
	public:
		AnnConsole();
		void append(std::string string);
		void setVisible(bool visibility);
		void toogle();
	private:
		void update();
		void WriteToTexture(const Ogre::String& str, Ogre::TexturePtr destTexture, Ogre::Image::Box destRectangle, Ogre::Font* font, const Ogre::ColourValue &color, char justify = 'l',  bool wordwrap = true);

		//////////////////////////////

		AnnVect3 points[4];
		AnnVect2 textCoord[4];

		std::string buffer[CONSOLE_BUFFER];
		bool visibility;
		Ogre::ManualObject* displaySurface;
		Ogre::TexturePtr displayTexture;
		Ogre::SceneNode* consoleNode;

		Ogre::TexturePtr texture;
		Ogre::FontPtr font;
		Ogre::TexturePtr background; 
	};
}

#endif