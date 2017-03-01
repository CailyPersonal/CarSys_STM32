///////////////////////////// start of file ///////////////////////////////////
/******************************************************************************
Module:  main.c
Notices: Copyright (c) 2015 CailyPersonal 
******************************************************************************/

//Include the standard Windows C++ and C-Runtime header files here.
#include "PublicHeader.h"
#include "stm32f10x.h"
#include "GeneralCtrl.h"
#include "WirelessConnect.h"
#include "Functions.h"
#include "Time.h"
///////////////////////////////////////////////////////////////////////////////
#define DEFAULT_UPLOAD_SPEED			15
#define SYSTEM_SIMPLE_MODE



int main(void){	
	u16 in=0;
	RCC->APB2ENR |= 1<<3;
	RCC->APB2ENR |= 1<<2;


	GPIOA->CRL &= 0x00;
//	GPIOA->CRL |= 0x10;

	
	GPIOB->CRH &=  0x07;
	GPIOA->CRL =  0x344;
	
	
	while(1)
	{
		in = GPIOA->IDR;

		GPIOA->ODR = in<<2;;
	}
	
	
	
	
	InitAllDevices();
	ConnectToWINCC();
	
	ApplyByCMD();
	while(1);
}


////////////////////////////  end of file  ////////////////////////////////////
