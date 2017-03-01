///////////////////////////// start of file ///////////////////////////////////
/******************************************************************************
Module:  WheelCtrl.cpp
Notices: 1.Copyright (c) 2015 CailyPersonal 
******************************************************************************/

//Include the standard Windows C++ and C-Runtime header files here.
#include "stm32f10x.h"
#include "WheelCtrl.h"
#include "PublicDataCtrl.h"

extern struct sCarInfo CarInfo;

u8 uWheelOrignSpeed			= 0x00;		/* 1.This variable is used to recovery speed
										   2.Before using turn function, a cancel process is necessary
										   3.This is designed to be using together with "uWheelTurnDirection" */
u8 uWheelTurnDirection		= 0x00;		/* 1.This variable is used to recovery speed */									  

void Wheel_DriverInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	SystemInit();
	/* RCC initilization */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 
	|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);
	
	/* GPIO initilization */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM1,DISABLE);
	
	/* GPIO initilization */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	/* TIM initilization */
	TIM_TimeBaseStruct.TIM_Period = 5000;//计数器重载
	TIM_TimeBaseStruct.TIM_Prescaler = 0;//预分频
	TIM_TimeBaseStruct.TIM_ClockDivision = 0;//时钟分割
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStruct);
	
	/* PWM msg initilization */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	
	
	/* Regisiter pwm msg info*/
	TIM_OC1Init(TIM1,&TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);
	TIM_OC2Init(TIM1,&TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM1,TIM_OCPreload_Enable);
	TIM_OC3Init(TIM1,&TIM_OCInitStructure);	
	TIM_OC3PreloadConfig(TIM1,TIM_OCPreload_Enable);
	TIM_OC4Init(TIM1,&TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM1,TIM_OCPreload_Enable);	
	
	//Enable pwm msg.
	TIM_Cmd(TIM1,ENABLE);

	//Output pwm msg.
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	Wheel_Direction_Forward();
	
	TIM_SetCompare1(TIM1, 0x00);
	TIM_SetCompare2(TIM1, 0x00);
	TIM_SetCompare3(TIM1, 0x00);
	TIM_SetCompare4(TIM1, 0x00);
}

void Wheel_SetRunSpeed(u8 uSpeed)
{
	Data_SetWheelsSpeeds(uSpeed, uSpeed, uSpeed, uSpeed);
	/* Set new speed to register */
	TIM_SetCompare3(TIM1,uSpeed*16);//back left
	TIM_SetCompare4(TIM1,uSpeed*16);//front left
	TIM_SetCompare1(TIM1,uSpeed*16);//front right
	TIM_SetCompare2(TIM1,uSpeed*16);//back right
}

void Wheel_CarRun(u8 uSpeed)
{
	Wheel_SetRunSpeed(uSpeed);
	EXTI ->IMR = 0x17;
}

void Wheel_CarStop(void)
{
	Wheel_SetRunSpeed(0x00);
	EXTI ->IMR  =  0x00;
}

void Wheel_Direction_Forward(void)
{
	Data_SetWheelsDirections(LEFT_FORWARD, RIGHT_FORWARD, LEFT_FORWARD, RIGHT_FORWARD);
	GPIO_SetBits(GPIOB,GPIO_Pin_9|GPIO_Pin_11);
	GPIO_ResetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_10);
}

void Wheel_Direction_Backward(void)
{
	Data_SetWheelsDirections(LEFT_BACKWARD, RIGHT_BACKWARD, LEFT_BACKWARD, RIGHT_BACKWARD);
	GPIO_ResetBits(GPIOB,GPIO_Pin_9|GPIO_Pin_11);
	GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_10);
}

void Wheel_SimpleTurnToLeft()
{
	Wheel_CarRun(0x70);
	Data_SetWheelsDirections(LEFT_BACKWARD, RIGHT_FORWARD, LEFT_BACKWARD, RIGHT_FORWARD);
	GPIO_SetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_11);
	GPIO_ResetBits(GPIOB,GPIO_Pin_9|GPIO_Pin_10);
	Wheel_CarRun(0xAA);
}

void Wheel_SimpleTurnToRight()
{
	Wheel_CarRun(0x70);
	Data_SetWheelsDirections(LEFT_FORWARD, RIGHT_BACKWARD, LEFT_FORWARD, RIGHT_BACKWARD);
	GPIO_SetBits(GPIOB,GPIO_Pin_9|GPIO_Pin_10);
	GPIO_ResetBits(GPIOB,GPIO_Pin_8|GPIO_Pin_11);
	Wheel_CarRun(0xAA);
}

void Wheel_SimpleStopTurn()
{
	Wheel_Direction_Forward();
}

void Wheel_SetAllWheelSpeed(u8 uSpeedLF, u8 uSpeedRF, u8 uSpeedLB, u8 uSpeedRB)
{
	Data_SetSingleWheelSpeed(LEFT_FRONT, uSpeedLF);
	Data_SetSingleWheelSpeed(RIGHT_FRONT, uSpeedRF);
	Data_SetSingleWheelSpeed(LEFT_BACK, uSpeedLB);
	Data_SetSingleWheelSpeed(RIGHT_BACK, uSpeedRB);
	TIM_SetCompare1(TIM1,uSpeedLF);
	TIM_SetCompare2(TIM1,uSpeedRF);
	TIM_SetCompare3(TIM1,uSpeedLB);
	TIM_SetCompare4(TIM1,uSpeedRB);
}

////////////////////////////  end of file  ////////////////////////////////////
