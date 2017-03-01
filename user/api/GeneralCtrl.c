#include "GeneralCtrl.h"
#include "COMCommunicate.h"
#include "WheelCtrl.h"
#include "ObstacleDeal.h"
#include "stm32f10x.h"

void MsgSoftWareInterruptInit()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	//¿ªÆôÖÐ¶ÏÔÊÐí
	EXTI->IMR |= 0x10;
	EXTI->EMR |= 0x10;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void InitAllDevices()
{
	COM_DriverInit();
	Wheel_DriverInit();
	Obstacle_DriverInit();
	MsgSoftWareInterruptInit();
}



















