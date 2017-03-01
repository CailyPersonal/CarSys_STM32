///////////////////////////// start of file ///////////////////////////////////
/******************************************************************************
Module:  WheelCtrl.h
Notices: 1.Copyright (c) 2015 CailyPersonal 
		 2.This file should be included in "Functions.h"
******************************************************************************/

//Include the standard Windows C++ and C-Runtime header files here.
#include "PublicHeader.h"

//////////////////// variables are defined here //////////////////////////////

///////////////////////////////////////////////////////////////////////////////
void Wheel_DriverInit(void);

void Wheel_SetRunDirection(u8 uDirection);
void Wheel_SetRunSpeed(u8 uSpeed);

void Wheel_CarRun(u8 uSpeed);
void Wheel_CarStop(void);

void Wheel_Direction_Forward(void);
void Wheel_Direction_Backward(void);

void Wheel_SimpleTurnToLeft(void);
void Wheel_SimpleTurnToRight(void);
void Wheel_SimpleStopTurn(void);

void Wheel_SetAllWheelSpeed(u8 uSpeedLF, u8 uSpeedRF, u8 uSpeedLB, u8 uSpeedRB);
////////////////////////////  end of file  ////////////////////////////////////
