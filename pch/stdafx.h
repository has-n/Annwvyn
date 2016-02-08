//Remove some visual studio stupidity
#pragma warning (disable : 4244)

//C STDLIB for C++
#include <cmath>
#include <cassert>
#include <cctype>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>

//C++ STL and STDLIB
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

//Object-Oriented Graphical Rendering Engine
#include <Ogre.h>
#include <OgrePrerequisites.h>
#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <OgreSceneNode.h>
#include <OgreEntity.h>
#include <OgreLight.h>
#include <OgreMatrix3.h>

#include <Overlay/OgreFont.h>
#include <Overlay/OgreFontManager.h>


//Object-Oriented Input System
#include <OIS.h>

//Bullet
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <btBulletDynamicsCommon.h>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btVector3.h>


//OpenAl
#include <al.h>
#include <alc.h>

//libsndfile
#include <sndfile.h>

//Oculus VR API
#include <OVR.h>
#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <glew.h>
#endif