///////////////////////////// start of file ///////////////////////////////////
/******************************************************************************
Module:  Functions.cpp
Notices: 1.Copyright (c) 2015 CailyPersonal 
******************************************************************************/

//Include the standard Windows C++ and C-Runtime header files here.
#include "stm32f10x.h"
#include "Functions.h"
#include "WheelCtrl.h"
#include "COMCommunicate.h"
#include "KernelProcess.h"
#include "PublicDataCtrl.h"
#include "ObstacleDeal.h"

extern BOOL bWINCCCmdIsBusy;
extern u8 uBuffer[100];


struct sTask Tasks[16];
int iRealAx = 0;
int iRealAy = 0;

///////////////////////////////////////////////////////////////////////////////
void WheelInitilization(){
	Wheel_DriverInit();			/* Driver initilize */
	Wheel_VariableInit();		/* Variable initilize */
	Wheel_CarStop();			/* Set car stop */
}

void COMInitilization(){
	COM_DriverInit();			/* Driver initilize */
	COM_VariableInit();			/* Variable initilize */
}

/* This part is not necessary now */
void SPIInitilization(){
	//To add code later
	return;
}

/* This part is not necessary now */
void ARHSInitilization(u8 uUploadSpeed){
	return;
}

void ObstacleInitilization(){
	Obstacle_DriverInit();


}
	
	
	
	
void AutoRunCar(){

}


void ApplyByCMD(){
	u8 uWINCCCmd[7] = {0};
	u8 uCount1 = 0;
	RegisterCarToWINCC();
	
	while(1){
		
		/* Get Msg of WINCC */
		if(bWINCCCmdIsBusy){
			uCount1 = 0;
			/* Save the Msg and open the switch */
			for(;uCount1<7;uCount1++){
				uWINCCCmd[uCount1] = uBuffer[uCount1+1];
			}
			uBuffer[0] = 0x00;
			bWINCCCmdIsBusy = FALSE;
		
			/* Deal with the cmd */
			switch(uWINCCCmd[0]){
				/* Give the point of the car real time */
				case 16:
					iRealAx = uWINCCCmd[1]*100 + uWINCCCmd[2];
					iRealAy = uWINCCCmd[3]*100 + uWINCCCmd[4];
					break;
				
				/* Set the speed of the car */
				case 17:
					uCount1 = uWINCCCmd[1];
					switch(uCount1){
						case 0x00:
							uCount1 = 255;
							break;
						case 0x01:
							uCount1 = 200;
							break;
						case 0x02:
							uCount1 = 145;
							break;
						case 0x03:
							uCount1 = 100;
							break;
					}
					Wheel_SetRunSpeed(uCount1);
					break;
				
				/* Run the car */
				case 19:
					LineAmend();
					//Wheel_CarRun(0xFF);
					break;
				
				case 20:
					Wheel_CarStop();
					break;
				
				case 21:
					Wheel_SimpleTurnToLeft();
					break;
				
				case 22:
					Wheel_SimpleTurnToRight();
					break;
				
				case 23:
					Wheel_SimpleStopTurn();
					break;
				
				
				case 0xEE:
					AbsoluteLine();
					break;
				
				
				case 0xFF:
					ModifyDegree();
					Wheel_SimpleStopTurn();
					break;
				
				
				default:
					
					/* Save the task and wait for the cmd */
					if(uWINCCCmd[0] < 16){
						Tasks[uWINCCCmd[0]].bIsDone	= FALSE;
						Tasks[uWINCCCmd[0]].iAx			= uWINCCCmd[1]*100 + uWINCCCmd[2];
						Tasks[uWINCCCmd[0]].iAy			= uWINCCCmd[3]*100 + uWINCCCmd[4];
						Tasks[uWINCCCmd[0]].bIsContinue	= uWINCCCmd[5];
					}
				}
				
				/* Clear the buffer and wait fot the new cmd */
				uCount1 = 0;
				for(;uCount1<6;uCount1++){
					uWINCCCmd[uCount1] = 0;
				}
		}
	}
}






















////////////////////////////  end of file  ////////////////////////////////////
