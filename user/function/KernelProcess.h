///////////////////////////// start of file ///////////////////////////////////
/******************************************************************************
Module:  KernelProcess.h
Notices: 1.Copyright (c) 2015 CailyPersonal 
******************************************************************************/

//Include the standard Windows C++ and C-Runtime header files here.
#include "PublicHeader.h"



///////////////////////////////////////////////////////////////////////////////
void ModifyDegree(void);
void CalculateLine(int sX, int sY, int eX, int eY);
float CalculateDegree(int iTx, int iTy);
float CalculateDegreeDeviation(float fCrDegree, float fTgDegree);
u8 LineAmend(void);
BOOL DeviationCalculate(void);
BOOL ObstacleDeal(u8 uIndex);
float Invsqrt(float uMath);
void FullfilSedDegreeBuffer(float fDegree);
void AbsoluteLine(void);
void HandleControl(u16 x, u16 y);
////////////////////////////  end of file  ////////////////////////////////////
