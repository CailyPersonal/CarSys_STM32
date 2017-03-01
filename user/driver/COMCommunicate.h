///////////////////////////// start of file ///////////////////////////////////
/******************************************************************************
Module:  COMCommunicate.h
Notices: 1.Copyright (c) 2015 CailyPersonal 
******************************************************************************/

//Include the standard Windows C++ and C-Runtime header files here.
#include "PublicHeader.h"



//////////////////// functions are defined here //////////////////////////////
void COM_DriverInit(void);
void COM_VariableInit(void);
void COM_WINCCPortConfiguration(void);
void COM_OuterPortConfiguration(void);
void COM_InnerClockConfiguration(void);
void COM_SetSwitchMode(u8 uIndex, BOOL bSwtich);
float COM_GetAvgDirection(u8 uIndex, u8 pData[]);
BOOL COM_GetPresPos(int uX, int uY);

BOOL COM_SendMessageToWINCC(u8 length, u8 pData[]);
void COM_SendMessageToOuterDevice(u8 length, u8 data[]);
int COM_FailToSend(u8 uIndex, u8 pData[]);


void COM_OuterDeviceHandler(void);
void COM_WINCCDeviceHandler(void);
void FeedBackToWINCC(void);
/////////////////////// all variables defined here ///////////////////////////


////////////////////////////  end of file  ////////////////////////////////////
