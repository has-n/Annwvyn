// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "stdafx.h"
#ifdef _WIN32
#include "AnnOculusInterfaceHelper.hpp"
#include "AnnLogger.hpp"
#include "AnnException.hpp"
#include "AnnEngine.hpp"

using namespace std;
using namespace OVR;
using namespace Annwvyn;

void AnnOculusInterfaceHelper::abortOnFailure()
{
	session = {};
	luid	= {};
	hmdDesc = {};

	//Notify user
	AnnDebug() << "Error: Cannot create Oculus Session";
	//Debug HMD is now handled by the configuration utility and the runtime.
	AnnDebug() << "Please make sure Oculus Home is installed on your system and "
				  "please check if you have correctly plugged HDMI and USB on the Rift and Tracker";

	displayWin32ErrorMessage("Error: Cannot create Oculus Session!",
							 "Please make sure Oculus Home is installed on your system\n"
							 "and check HDMI and USB connection to your Rift and Tracker");

	//Cleanup
	ovr_Shutdown();
	//Return an error
	AnnDebug() << "Unable to Initialize client library or get a session valid from the Oculus Runtime. Closing program and returning 0xDEAD60D error";
	//Stop program
	throw AnnInitializationError(ANN_ERR_CRITIC, "Unable to create an Oculus session");
}

AnnOculusInterfaceHelper::AnnOculusInterfaceHelper()
{
	AnnDebug() << "Init Oculus Interface object";

	ovrInitParams params = { ovrInit_FocusAware };

	//Init Oculus Virtual Reality library
	if(OVR_FAILURE(ovr_Initialize(&params)))
		abortOnFailure();

	//Declare this client to the Oculus service
	stringstream clientIentifier;
	clientIentifier << "EngineName: Annwvyn\n";
	clientIentifier << "EngineVersion: " << AnnEngine::getAnnwvynVersion();
	ovr_IdentifyClient(clientIentifier.str().c_str());
	AnnDebug() << "Identifier string sent to the Oculus Service : \n"
			   << clientIentifier.str();
	AnnDebug() << "LibOVR version : " << ovr_GetVersionString();

	//Attempt to create OVR session
	if(OVR_FAILURE(ovr_Create(&session, &luid)))
		abortOnFailure();

	//Fill the hmdDesc structure
	hmdDesc = ovr_GetHmdDesc(session);

	//Print to log all known information about the headset
	logHardwareReport();
}

AnnOculusInterfaceHelper::~AnnOculusInterfaceHelper()
{
	//Set the performance HUD to Off
	ovr_SetInt(getSession(), "PerfHudMode", ovrPerfHud_Off);

	AnnDebug() << "Shutdown AnnOculusInterfaceHelper object";
	ovr_Destroy(getSession());
	ovr_Shutdown();
	AnnDebug() << "LibOVR Shutdown... No longer can communicate with OculusService";
}

inline Ogre::Vector3 oculusToOgreVect3(const ovrVector3f& v) { return { v.x, v.y, v.z }; }
inline Ogre::Quaternion oculusToOgreQuat(const ovrQuatf& q) { return { q.w, q.x, q.y, q.z }; }

void AnnOculusInterfaceHelper::logHardwareReport() const
{
	//Print to the logger a bunch of information
	AnnDebug() << " - Detected Oculus hardware :";
	AnnDebug() << "OVR version " << ovr_GetVersionString();
	AnnDebug() << "Detected the following Oculus Rift VR Headset :";
	AnnDebug() << "Product name : " << hmdDesc.ProductName;
	AnnDebug() << "Serial number : " << hmdDesc.SerialNumber;
	AnnDebug() << "Manufacturer : " << hmdDesc.Manufacturer;
	AnnDebug() << "Display Resolution : " << hmdDesc.Resolution.w << "x" << hmdDesc.Resolution.h;
	AnnDebug() << "Display refresh rate : " << hmdDesc.DisplayRefreshRate << "Hz";
	AnnDebug() << "Type of HMD identifier : " << hmdDesc.Type;
	AnnDebug() << "Firmware version : " << hmdDesc.FirmwareMajor << "." << hmdDesc.FirmwareMinor;

	const auto trackerCount{ ovr_GetTrackerCount(session) };
	AnnDebug() << "Detected " << trackerCount << " Oculus Sensors";
	for(auto i{ 0u }; i < trackerCount; ++i)
	{
		const auto tracker	 = 1 + i;
		const auto trackerPose = ovr_GetTrackerPose(session, i).Pose;
		const auto desc		   = ovr_GetTrackerDesc(session, i);

		AnnDebug() << "Tracker_" << tracker << " Pose : " << oculusToOgreVect3(trackerPose.Position) << "; " << oculusToOgreQuat(trackerPose.Orientation);

		AnnDebug() << "Tracker_" << tracker << " Camera Frustum : Near "
				   << desc.FrustumNearZInMeters << "m Far "
				   << desc.FrustumFarZInMeters << "m H fov "
				   << desc.FrustumHFovInRadians << "rad V fov "
				   << desc.FrustumVFovInRadians << "rad";
	}
}

ovrHmdDesc AnnOculusInterfaceHelper::getHmdDesc() const
{
	return hmdDesc;
}

ovrSession AnnOculusInterfaceHelper::getSession() const
{
	return session;
}

float AnnOculusInterfaceHelper::getUserEyeHeight() const
{
	return ovr_GetFloat(session, "EyeHeight", -1.f);
}

void AnnOculusInterfaceHelper::recenterTrackingOrigin() const
{
	ovr_RecenterTrackingOrigin(session);
}

void AnnOculusInterfaceHelper::setPerfHudMode(ovrPerfHudMode mode) const
{
	ovr_SetInt(session, "PerfHudMode", int(mode));
}

ovrSizei AnnOculusInterfaceHelper::getHmdResolution() const
{
	return hmdDesc.Resolution;
}

float AnnOculusInterfaceHelper::getHmdDisplayRefreshRate() const
{
	return hmdDesc.DisplayRefreshRate;
}

void AnnOculusInterfaceHelper::setTrackingOriginToFloorLevel() const
{
	ovr_SetTrackingOriginType(session, ovrTrackingOrigin_FloorLevel);
}

#endif
