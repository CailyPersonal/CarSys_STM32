///////////////////////////// start of file ///////////////////////////////////
/******************************************************************************
Module:  PublicDataCtrl.h
Notices: 1.Copyright (c) 2015 CailyPersonal 
		 2.All variables are contained in this file.
******************************************************************************/

//Include the standard Windows C++ and C-Runtime header files here.
#include "PublicHeader.h"

////////////////// Static debug variables defined here ////////////////////////

enum WheelName{
	LEFT_FRONT,
	RIGHT_FRONT,
	LEFT_BACK,
	RIGHT_BACK
};

enum WheelDirecion{
	LEFT_FORWARD,
	LEFT_BACKWARD,
	RIGHT_FORWARD,
	RIGHT_BACKWARD
};


enum CarDirection{
	CAR_STOP,
	CAR_FORWARD,
	CAR_BACKWARD,
	CAR_NONESYNCHRONOUS
};

struct WheelSpeeds
{
	u8	LeftFront, RightFront, LeftBack, RightBack;
};

struct WheelDirecions
{
	enum	WheelDirecion	LeftFront, RightFront, LeftBack, RightBack;
};

struct CarInfo{
	struct	WheelSpeeds	wheelSpeeds;
	struct	WheelDirecions	wheelDirections;
	u8		TaskCount, TaskPresentIndex;
	float	Yaw;
};


///////////////// Data transfer functions are defined here ////////////////////

void InitBuffer(void);
void Data_SetSingleWheelSpeedAndDirection(u8 speed, enum WheelDirecion direction, enum WheelName name);
void Data_SetCarSpeedAndDirection(u8 speed, enum CarDirection carDirection);
void Data_SetTasks(u8 count);
void Data_SetTaskPresentIndex(u8 index);
void Data_SetPresentYaw(float yaw);
void Data_SetSingleWheelDirection(enum WheelName name, enum WheelDirecion direction);
void Data_SetSingleWheelSpeed(enum WheelName name, u8 speed);
void Data_SetWheelsDirections(enum WheelDirecion LF, enum WheelDirecion RF, 
	enum WheelDirecion LB, enum WheelDirecion RB);
void Data_SetWheelsSpeeds(u8 LF, u8 RF, u8 LB, u8 RB);
u8 GetWheelSpeed(enum WheelName name);
u8 GetWheelDirection(void);
////////////////////////////  end of file  ////////////////////////////////////
