///////////////////////////// start of file ///////////////////////////////////
/******************************************************************************
Module:  PublicHeader.h
Notices: 1.Copyright (c) 2015 CailyPersonal 
		 2.This file needs to be included in each cpp file
		 3.Key headers need to be inclued in the file
******************************************************************************/

//Include the standard Windows C++ and C-Runtime header files here.
#include <stdbool.h>


/* exact-width unsigned integer types */
typedef unsigned          char uint8_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;
typedef bool								BOOL;



#define CAR_BOARD_ID       0x14


typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

#define TRUE 0x01
#define FALSE 0x00




#define CAR_BASIC_RUN_MODE										0x00
#define CAR_RUN_SPEED											0x01
#define CAR_WHEEL_LEFT_FRONT									0x02
#define CAR_WHEEL_RIGHT_FRONT									0x03
#define CAR_WHEEL_LEFT_BACK										0x04
#define CAR_WHEEL_RIGHT_BACK									0x05



#define COM_WINCC												0x00
#define COM_OUTERDEVICE											0x01

typedef u8 TYPE;


//Define run enviroment by user.


///////////////////////////////////////////////////////////////////////////////
void TimeDelay(u8 uTime);


////////////////////////////  end of file  ////////////////////////////////////


