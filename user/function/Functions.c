///////////////////////////// start of file ///////////////////////////////////
/******************************************************************************
Module:  Functions.cpp
Notices: 1.Copyright (c) 2015 CailyPersonal 
******************************************************************************/

//Include the standard Windows C++ and C-Runtime header files here.
#include "stm32f10x.h"
#include "Functions.h"
#include "WirelessConnect.h"
#include "CarRunCtrl.h"
#include "KernelProcess.h"
#include "DirectionModel.h"
#include "Time.h"
struct	sTask Tasks[16];
extern	u8	uWINCCCmd[6];
u16 x,y;
float fCrDegree = 0;

void AutoRunCar(){

}

void ApplyByCMD()
{	
//	u16 k;
	u8 TasksCount, Temp;//-------------------------------------------------------------------
	SetZeroDegree();
	TasksCount = 0;
	while(1)
	{
//		k++;
//		if(1 == k)
//		{
//			fCrDegree = GetCurrentDirection();
//			SendCarYawAndSpeedToWINCC(fCrDegree);
//		}
		if(TryToGetMsgFromWINCC())
		{
			switch(uWINCCCmd[0])
			{		
			case 0x12:
				SuperviseAllTaskCount(TasksCount);//------------------------------------------
				SuperviseAllTaskCount(TasksCount);//------------------------------------------
				for(Temp = 0; Temp < TasksCount; Temp++)//------------------------------------
					SuperviseTaskDetail(Temp, Tasks[Temp].iAx, Tasks[Temp].iAy);//------------
				LineAmend();
				TasksCount = 0;//-------------------------------------------------------------
				break;
			case 0x13:
				
			case 0x14:
				CarStop();
				break;
			case 0x15:
				CarForward(0xAA);
				break;
			case 0x16:
				CarBackward(0xAA);
				break;
			case 0x21:
				CarTurnLeft(0x80);
				break;
			case 0x22:
				CarTurnRight(0x80);
				break;
			
			case 0x23:	
				x = uWINCCCmd[1];
				x = x << 8;
				x += uWINCCCmd[2];
				y = uWINCCCmd[3];
				y = y<< 8;
				y += uWINCCCmd[4];
				SuperviseAllTaskCount(1);//------------------------------------------
				SuperviseTaskDetail(0, x, y);
			
			
				HandleControl(x, y);
				break;
			default:
				if(uWINCCCmd[0] < 0x10)
				{
					Tasks[uWINCCCmd[0]].iAx = uWINCCCmd[1];
					Tasks[uWINCCCmd[0]].iAx = Tasks[uWINCCCmd[0]].iAx << 8;
					Tasks[uWINCCCmd[0]].iAx += uWINCCCmd[2];
					Tasks[uWINCCCmd[0]].iAy = uWINCCCmd[3];
					Tasks[uWINCCCmd[0]].iAy = Tasks[uWINCCCmd[0]].iAy << 8;
					Tasks[uWINCCCmd[0]].iAy += uWINCCCmd[4];
					Tasks[uWINCCCmd[0]].bIsContinue = uWINCCCmd[5];
				}
				TasksCount++;
				break;
			}//End switch
		}
	}//End while
}






















////////////////////////////  end of file  ////////////////////////////////////
