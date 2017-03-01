///////////////////////////// start of file ///////////////////////////////////
/******************************************************************************
Module:  PublicDataCtrl.cpp
Notices: 1.Copyright (c) 2015 CailyPersonal 
******************************************************************************/

//Include the standard Windows C++ and C-Runtime header files here.
#include "stm32f10x.h"
#include "PublicDataCtrl.h"
///////////////////////////////////////////////////////////////////////////////

////////////////////// Status save buffer is defined here /////////////////////
struct CarInfo carInfo;

////////////////////// Functions are declared here ////////////////////////////

/************************ InitBuffer **************************
Name  : InitBuffer
Param : None
Notice: Initilitize all variables
**************************************************************/
void InitBuffer(void){
	carInfo.wheelSpeeds.LeftBack		= 0x00;
	carInfo.wheelSpeeds.LeftFront		= 0x00;
	carInfo.wheelSpeeds.RightBack		= 0x00;
	carInfo.wheelSpeeds.RightFront		= 0x00;
	carInfo.wheelDirections.LeftBack	= LEFT_FORWARD;
	carInfo.wheelDirections.LeftFront	= LEFT_FORWARD;
	carInfo.wheelDirections.RightBack	= RIGHT_FORWARD;
	carInfo.wheelDirections.RightFront	= RIGHT_FORWARD;
	carInfo.TaskCount					= 0;
	carInfo.TaskPresentIndex			= 0;
	carInfo.Yaw							= 0;
}

void Data_SetSingleWheelSpeedAndDirection(u8 speed, enum WheelDirecion direction, enum WheelName name)
{
	switch(name)
	{
		case LEFT_FRONT:
			carInfo.wheelSpeeds.LeftFront = speed;
			carInfo.wheelDirections.LeftFront = direction;
			break;
		case RIGHT_FRONT:
			carInfo.wheelSpeeds.RightFront = speed;
			carInfo.wheelDirections.RightFront = direction;
			break;
		case LEFT_BACK:
			carInfo.wheelSpeeds.LeftBack = speed;
			carInfo.wheelDirections.LeftBack = direction;
			break;
		case RIGHT_BACK:
			carInfo.wheelSpeeds.RightBack = speed;
			carInfo.wheelDirections.RightBack = direction;
			break;
	}
}	

void Data_SetCarSpeedAndDirection(u8 speed, enum CarDirection carDirection)
{
	carInfo.wheelSpeeds.LeftBack		= speed;
	carInfo.wheelSpeeds.LeftFront		= speed;
	carInfo.wheelSpeeds.RightBack		= speed;
	carInfo.wheelSpeeds.RightFront		= speed;
	switch(carDirection)
	{
		case CAR_FORWARD:
			carInfo.wheelDirections.LeftBack	= LEFT_FORWARD;
			carInfo.wheelDirections.LeftFront	= LEFT_FORWARD;
			carInfo.wheelDirections.RightBack	= RIGHT_FORWARD;
			carInfo.wheelDirections.RightFront	= RIGHT_FORWARD;		
			break;
		case CAR_BACKWARD:
			carInfo.wheelDirections.LeftBack	= LEFT_BACKWARD;
			carInfo.wheelDirections.LeftFront	= LEFT_BACKWARD;
			carInfo.wheelDirections.RightBack	= RIGHT_BACKWARD;
			carInfo.wheelDirections.RightFront	= RIGHT_BACKWARD;
			break;
		case CAR_STOP:
			carInfo.wheelSpeeds.LeftBack		= 0x00;
			carInfo.wheelSpeeds.LeftFront		= 0x00;
			carInfo.wheelSpeeds.RightBack		= 0x00;
			carInfo.wheelSpeeds.RightFront		= 0x00;
			break;
		case CAR_NONESYNCHRONOUS:
			break;
	}
}
	
void Data_SetTasks(u8 count)
{
	carInfo.TaskCount = count;
}

void Data_SetTaskPresentIndex(u8 index)
{
	carInfo.TaskPresentIndex = index;
}

void Data_SetPresentYaw(float yaw)
{
	carInfo.Yaw = yaw;
}

void Data_SetSingleWheelDirection(enum WheelName name, enum WheelDirecion direction)
{
	switch(name)
	{
		case LEFT_BACK:
			carInfo.wheelDirections.LeftBack	= direction;
			break;
		case LEFT_FRONT:
			carInfo.wheelDirections.LeftFront	= direction;
			break;
		case RIGHT_BACK:
			carInfo.wheelDirections.RightBack	= direction;
			break;
		case RIGHT_FRONT:
			carInfo.wheelDirections.RightFront	= direction;
			break;
	}
}	

void Data_SetSingleWheelSpeed(enum WheelName name, u8 speed)
{
	switch(name)
	{
		case LEFT_BACK:
			carInfo.wheelSpeeds.LeftBack	= speed;
			break;
		case LEFT_FRONT:
			carInfo.wheelSpeeds.LeftFront	= speed;
			break;
		case RIGHT_BACK:
			carInfo.wheelSpeeds.RightBack	= speed;
			break;
		case RIGHT_FRONT:
			carInfo.wheelSpeeds.RightFront	= speed;
			break;
	}
}

void Data_SetWheelsDirections(enum WheelDirecion LF, enum WheelDirecion RF, 
	enum WheelDirecion LB, enum WheelDirecion RB)
{
	carInfo.wheelDirections.LeftFront	= LF;
	carInfo.wheelDirections.RightFront	= RF;
	carInfo.wheelDirections.LeftBack	= LB;
	carInfo.wheelDirections.RightBack	= RB;
}

void Data_SetWheelsSpeeds(u8 LF, u8 RF, u8 LB, u8 RB)
{
	carInfo.wheelSpeeds.LeftFront	= LF;
	carInfo.wheelSpeeds.RightFront	= RF;
	carInfo.wheelSpeeds.LeftBack	= LB;
	carInfo.wheelSpeeds.RightBack	= RB;
}

u8 GetWheelSpeed(enum WheelName name)
{
	switch(name)
	{
		case LEFT_BACK:
			return carInfo.wheelSpeeds.LeftBack;
		case LEFT_FRONT:
			return carInfo.wheelSpeeds.LeftFront;
		case RIGHT_BACK:
			return carInfo.wheelSpeeds.RightBack;
		case RIGHT_FRONT:
			return carInfo.wheelSpeeds.RightFront;
	}
	return 0x00;
}

u8 GetWheelDirection()											
{
	u8 Direction;
	Direction |= carInfo.wheelDirections.LeftFront << 6;
	Direction |= carInfo.wheelDirections.LeftBack << 4;
	Direction |= carInfo.wheelDirections.RightFront << 2;
	Direction |= carInfo.wheelDirections.RightBack;
	return Direction;
}

////////////////////////////  end of file  ////////////////////////////////////
