#include "stdafx.h"
#include "AnnPlayer.hpp"
#include "AnnEngine.hpp"
#include "AnnLogger.hpp"

using namespace Annwvyn;

bodyParams::bodyParams()
{
	//these parameters looks good for testing. Costumise them before initializing the physics!
	eyeHeight = 1.59f;
	walkSpeed = 3;
	turnSpeed = 0.5f;
	mass = 80.0f;
	Position = AnnVect3(0, 0, 10);
	HeadOrientation = AnnQuaternion(1, 0, 0, 0);
	Shape = NULL;
	Body = NULL;
	runFactor = 3;
	jumpForce = AnnVect3(0, 500, 0);
}

AnnPlayer::AnnPlayer()
{
	AnnDebug() << "Creating player object";
	playerBody = new bodyParams;
	locked = false;
	run = false;

	walking[forward] = false;
	walking[backward] = false;
	walking[left] = false;
	walking[right] = false;

	analogWalk = 0;
	analogStraff = 0;
	analogRotate = 0;
	
	for(size_t i(0); i < JMP_BUFFER; i++) YSpeedWasZero[i]=false;
	frameCount = 0;
	standing = true;
	updateTime = 0;
	physics = false;	
}

AnnPlayer::~AnnPlayer()
{
	delete playerBody;
}

bool AnnPlayer::isLocked()
{
	return locked;
}

void AnnPlayer::setPosition(AnnVect3 Position)
{
	playerBody->Position = Position;
}

void AnnPlayer::setOrientation(Ogre::Euler Orientation)
{
	playerBody->Orientation = Orientation;
}

void AnnPlayer::setHeadOrientation(AnnQuaternion Orientation)
{
	playerBody->HeadOrientation = Orientation;
}

void AnnPlayer::setEyesHeight(float eyeHeight)
{
	if(!isLocked())
		playerBody->eyeHeight = eyeHeight;
}

void AnnPlayer::setWalkSpeed(float walk)
{
	if(!isLocked())
		playerBody->eyeHeight = walk;
}

void AnnPlayer::setTurnSpeed(float ts)
{
	if(!isLocked())
		playerBody->eyeHeight = ts;
}

void AnnPlayer::setMass(float mass)
{
	if(!isLocked())
		playerBody->eyeHeight = mass;
}

void AnnPlayer::setShape(btCollisionShape* Shape)
{
	if(!isLocked())
		playerBody->Shape = Shape;
	physics = true;
}

void AnnPlayer::setBody(btRigidBody* Body)
{
//	if(!isLocked())
		playerBody->Body = Body;
		physics = true;
}

void AnnPlayer::lockParameters()
{
	locked = true;
}

void AnnPlayer::unlockParameters()
{
	locked = true;
}

bodyParams* AnnPlayer::getLowLevelBodyParams()
{
	return playerBody;
}

float AnnPlayer::getWalkSpeed()
{
	return playerBody->walkSpeed;
}

float AnnPlayer::getEyesHeight()
{
	return playerBody->eyeHeight;
}

float AnnPlayer::getTurnSpeed()
{
	return playerBody->turnSpeed;
}

float AnnPlayer::getMass()
{
	return playerBody->mass;
}

btRigidBody* AnnPlayer::getBody()
{
	return playerBody->Body;
}

btCollisionShape* AnnPlayer::getShape()
{
	return playerBody->Shape;
}

AnnVect3 AnnPlayer::getPosition()
{
	return playerBody->Position;
}

Ogre::Euler AnnPlayer::getOrientation()
{
	return playerBody->Orientation;
}

void AnnPlayer::applyRelativeBodyYaw(Ogre::Radian angle)
{
	playerBody->Orientation.yaw(angle);
}

void AnnPlayer::applyMouseRelativeRotation(int relValue)
{
	applyRelativeBodyYaw(Ogre::Radian(- float(relValue) * getTurnSpeed() * updateTime));
}

AnnVect3 AnnPlayer::getTranslation()
{
	AnnVect3 translation (AnnVect3::ZERO);
	if(walking[forward])
		translation.z -= 1;
	if(walking[backward])
		translation.z += 1;
	if(walking[left])
		translation.x -= 1;
	if(walking[right])
		translation.x += 1;

	return translation.normalisedCopy();
}

AnnVect3 AnnPlayer::getAnalogTranslation()
{
	AnnVect3 translate(AnnVect3::ZERO);

	translate.x = analogStraff;
	translate.z = analogWalk;

	return translate;
}

void AnnPlayer::applyAnalogYaw()
{
	//7 is the value that was more or less feeling good for me. 
	applyRelativeBodyYaw(Ogre::Radian(- 7 * analogRotate * getTurnSpeed() * updateTime));
}

void AnnPlayer::jump()
{
	if(!getBody()) return;
	if(contactWithGround)
		getBody()->applyCentralImpulse(playerBody->jumpForce.getBtVector());
}

void AnnPlayer::engineUpdate(float time)
{
	updateTime = time;
	//To "emulate" falling on the ground, set this boolean to false
	if(getBody())
	{
		frameCount++;
		applyAnalogYaw();
		AnnVect3 translate(getWalkSpeed() * (getTranslation() + getAnalogTranslation()));

		btVector3 currentVelocity = getBody()->getLinearVelocity();

		//The cast to an in is for eliminating "noise" from the reading of the Y componant of the speed vector
		//Apparently, event with the player standing on a plane, it's a really small number that is not strictly equals to zero.
		
		contactWithGround = false;
		
		/* --- don't care about jump here

		if(int(currentVelocity.y()) == 0)
		{
			bool canJump(true);
			for(size_t i(0); i < JMP_BUFFER; i++)
				if(!YSpeedWasZero[i]) canJump = false;

			contactWithGround = canJump;
			YSpeedWasZero[frameCount%JMP_BUFFER] = true;
		}
		else
		{
			YSpeedWasZero[frameCount%JMP_BUFFER] = false;
		}

		*/

		//Prevent the rigid body to be put asleep by the physics engine
		getBody()->activate();

		//if no  user input, be just pull toward the ground by gravity (not physicly realist, but usefull)
		if(translate.isZeroLength())
		{
			getBody()->setLinearVelocity(btVector3(0, currentVelocity.y(), 0));
		}
		else
		{
			AnnVect3 velocity(getOrientation()*translate);
			if(run) velocity *= playerBody->runFactor;
			getBody()->setLinearVelocity(btVector3(velocity.x, currentVelocity.y(), velocity.z));
		}

		//if the player can stand (= not dead or something like that)
		if(standing) 
		{
			btTransform Transform = getBody()->getCenterOfMassTransform();
			Transform.setRotation(AnnQuaternion(0, 0, 0, 1).getBtQuaternion());
			getBody()->setCenterOfMassTransform(Transform);
		}

		//get back position data from physics engine
		setPosition(
			AnnVect3( 
			getBody()->getCenterOfMassPosition().x(),
			getBody()->getCenterOfMassPosition().y() + getEyesHeight()/2,
			getBody()->getCenterOfMassPosition().z()
			));
	}
}

bool AnnPlayer::hasPhysics()
{
	return physics;
}
