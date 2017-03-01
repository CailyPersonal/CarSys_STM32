///////////////////////////// start of file ///////////////////////////////////
/******************************************************************************
Module:  Functions.h
Notices: 1.Copyright (c) 2015 CailyPersonal 
		 2.This file includes all the functions used in main.c file.
		 3.No other function excluded from this file is allowed in main.c
******************************************************************************/

//Include the standard Windows C++ and C-Runtime header files here.
#include "PublicHeader.h"
///////////////////////////////////////////////////////////////////////////////

void WheelInitilization(void);
void COMInitilization(void);
void SPIInitilization(void);
void ARHSInitilization(u8 uUploadSpeed);
void ObstacleInitilization(void);
void SetSpeed(u8 uSpeed,u8 uWheel,u8 uMode);
void SetSppedEx(u8 uSpeedLF, u8 uSpeedRF, u8 uSpeedLB, u8 uSpeedRB,
	u8 uDirectionLF, u8 uDirectionRF, u8 uDirectionLB, u8 uDirectionRB);
void SetDirection(u8 uDirection);	

void AutoRunCar(void);

void ApplyByCMD(void);
///////////////////////////////////////////////////////////////////////////////
struct sTask{
	BOOL	bIsDone;
	BOOL	bIsContinue;
	int		iAx;
	int		iAy;
};


////////////////////////////  end of file  ////////////////////////////////////
