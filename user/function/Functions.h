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

void AutoRunCar(void);

void ApplyByCMD(void);
///////////////////////////////////////////////////////////////////////////////
struct sTask{
	BOOL	bIsDone;
	BOOL	bIsContinue;
	u16		iAx;
	u16		iAy;
};

////////////////////////////  end of file  ////////////////////////////////////
