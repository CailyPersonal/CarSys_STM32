///////////////////////////// start of file ///////////////////////////////////
/******************************************************************************
Module:  COMCommunicate.cpp
Notices: 1.Copyright (c) 2015 CailyPersonal 
		 2.WINCC use USART1
		 3.OUTER use USART2
******************************************************************************/

//Include the standard Windows C++ and C-Runtime header files here.
#include "stm32f10x.h"
#include "COMCommunicate.h"

extern struct sCarInfo CarInfo;
//////////////////// variables are defined here //////////////////////////////
u8		uBuffer[15];
u8		uOuterDegreeBuffer[2];
u8		uResendTimes				= 0;
u8		uGetIndex					= 1;
u8		uGetDegreeIndex				= 0;
u8 		feedBack[3] 				= {0xA5, CAR_BOARD_ID, 0xFF};
bool 	bProcInSave 				= false;
/* This part is used to get the info of direction */
u8		uBufferFullCount			= 0;
BOOL	bGetDirection				= FALSE;
BOOL  	bWINCCCmdIsBusy				= FALSE;
BOOL  	IsCarRegistered				= FALSE;
///////////////////////////////////////////////////////////////////////////////
/*********************** COM_DriverInit ***********************
Name  : COM_DriverInit
Param : None
Notice: None
**************************************************************/
void COM_DriverInit(){
	/* Inner clock configure for all USART port communicate */
	COM_InnerClockConfiguration();

	/* Upper computer and outer device configuration */
	COM_WINCCPortConfiguration();
	COM_OuterPortConfiguration();
}

/********************** COM_VariableInit **********************
Name  : COM_VariableInit
Param : None
Notice: None
**************************************************************/
void COM_VariableInit(){
	//None
}

/***************** COM_WINCCPortConfiguration *****************
Name  : COM_WINCCPortConfiguration
Param : None
Notice: 1.The defalut set of this port is disable
		2.USART1 is used
**************************************************************/
void COM_WINCCPortConfiguration(){
	/* Notice: The defalut set of this port is disable */
	
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	/* RCC initilization */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOB|
			RCC_APB2Periph_AFIO, ENABLE);
	
	/* GPIO initilize */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//Set PB7 as Receive
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//Set PB6 as Send
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_PinRemapConfig(GPIO_Remap_USART1,ENABLE);//Remap
	
	/* NVIC initilization */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* USART initilization */
	USART_InitStructure.USART_BaudRate = 9600; //baudrate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //8 lenth of data
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // 1 stop bit
	USART_InitStructure.USART_Parity = USART_Parity_No; //No partity
	USART_InitStructure.USART_HardwareFlowControl =	USART_HardwareFlowControl_None; //No hardware flow ctrl
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; //Enable Tx and Rx
	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  //Enable interrupt

	USART_Cmd(USART1,ENABLE);
	USART_ClearFlag(USART1,USART_FLAG_TC);
}

/***************** COM_OuterPortConfiguration *****************
Name  : COM_OuterPortConfiguration
Param : None
Notice: The defalut set of this port is disable
**************************************************************/
void COM_OuterPortConfiguration(){
	/* Notice: The defalut set of this port is disable */
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	/* GPIO initilize */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//Set PA3 as Receive
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;//Set PA2 as Send
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_PinRemapConfig(GPIO_Remap_USART2,DISABLE);//Remap

	/* NVIC initilization */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* USART initilization */
	USART_InitStructure.USART_BaudRate = 38400; //baudrate = 38400;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //8 lenth of data
	USART_InitStructure.USART_StopBits = USART_StopBits_1; // 1 stop bit
	USART_InitStructure.USART_Parity = USART_Parity_No; //No partity
	USART_InitStructure.USART_HardwareFlowControl =	USART_HardwareFlowControl_None; //No hardware flow ctrl
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; //Enable Tx and Rx
	USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);  //Enable interrupt

	USART_Cmd(USART2,ENABLE);

	USART_ClearFlag(USART2,USART_FLAG_TC);
	USART_GetFlagStatus(USART2, USART_FLAG_TC);
}

/***************** COM_InnerClockConfiguration ****************
Name  : COM_InnerClockConfiguration
Param : None
Notice: This function is to set the clock of the port.
**************************************************************/
void COM_InnerClockConfiguration(){
	/* Initilize inner clock */
	SystemInit();
}

BOOL COM_SendMessageToWINCC(u8 length, u8 pData[])
{
	BOOL bReturn	= FALSE;
	u8 uTemp = 0;
	USART_GetFlagStatus(USART1, USART_FLAG_TC);
	for(uTemp =0 ; uTemp < length ; uTemp++)
	{
		USART_SendData(USART1,pData[uTemp]);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); 
	}
	return bReturn;
}

void COM_SendMessageToOuterDevice(u8 length, u8 data[])
{
	u8 uTemp = 0;
	USART_GetFlagStatus(USART2, USART_FLAG_TC);
	for(uTemp =0 ; uTemp < length ; uTemp++)
	{
		USART_SendData(USART2,data[uTemp]);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); 
	}
}

/***************** COM_OuterDeviceHandler *********************
Name  : COM_OuterDeviceHandler
Param : None
Notice: 1.This function is used to get outer device info
				2.This function is allowed to be run when the switch is on
**************************************************************/
void COM_OuterDeviceHandler(){
	u8	uTemp;
	if(USART_GetITStatus(USART2,USART_IT_RXNE)){
		uTemp = USART_ReceiveData(USART2);
		if(0 == uGetDegreeIndex){
			uOuterDegreeBuffer[uGetDegreeIndex++] = uTemp;
		}
		else{
			uOuterDegreeBuffer[1] = uTemp;
			uGetDegreeIndex				= 0;
			bGetDirection 				= TRUE;
		}
	}
}


void SpecialMsgDeal()
{
	switch(uBuffer[2])
	{
		case 0xFE:
			IsCarRegistered = true;
			break;
		case 0xFF:
			break;
	}
}

void FeedBackToWINCC()
{
	COM_SendMessageToWINCC(3, feedBack);
}


/***************** COM_WINCCDeviceHandler *********************
Name  : COM_WINCCDeviceHandler
Param : None
Notice: This function is used to get WINCC device data
**************************************************************/
void COM_WINCCDeviceHandler(){
	u8 uTemp;
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET){
		USART_ReceiveData(USART1);
	}	
	
	if(bWINCCCmdIsBusy)
		return;
	if(USART_GetITStatus(USART1,USART_IT_RXNE)){
		uTemp = USART_ReceiveData(USART1);
		if(!bProcInSave)
		{
			if(0xA5 == uTemp)
			{
				uBuffer[0] = 0xA5;
				bProcInSave = true;
				uGetIndex = 1;
			}
		}
		else
		{
			if(1 == uGetIndex)//判断该条信息是否为本车所接受
			{
				if(uTemp!=CAR_BOARD_ID)
				{
					bProcInSave = false;
				}
				uBuffer[uGetIndex++] = uTemp;
			}
			else if(2 == uGetIndex)//判断是否为特殊命令（3字节长度）
			{
				if(uTemp == 0xFF || uTemp == 0xFE)
				{
					uBuffer[2] = uTemp;
					bProcInSave = false;
					SpecialMsgDeal();//调用特殊处理函数
				}
				uBuffer[uGetIndex++] = uTemp;
			}
			else//接收其余命令
			{
				uBuffer[uGetIndex++] = uTemp;
				if(10 == uGetIndex)
				{
					bProcInSave = false;
					EXTI->SWIER |= 0x10;	
				}
			}
		}
	}
}

void USART1_IRQHandler(void){
	COM_WINCCDeviceHandler();
}

void USART2_IRQHandler(void){
	COM_OuterDeviceHandler();
}
////////////////////////////  end of file  ////////////////////////////////////
