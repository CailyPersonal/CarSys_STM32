///////////////////////////// start of file ///////////////////////////////////
#include "WirelessConnect.h"
#include "COMCommunicate.h"
#include "PublicDataCtrl.h"
#include "stm32f10x.h"
#include "Time.h"

extern	BOOL	IsCarRegistered;
extern	BOOL	bWINCCCmdIsBusy;
extern	u8		uBuffer[15];
extern 	bool	EmergencyStop;
BOOL	NewMsgFromWINCC	= FALSE;
u8	ApplyToRegister[3]			= {0xA5, CAR_BOARD_ID, 0xFE};//申请注册本小车
u8	ErrorDuringTransfer[3]		= {0xA5, CAR_BOARD_ID, 0x30};//通知上位机前次发送消息出错
u8	CarInfoBuffer[10] = {0xA5, CAR_BOARD_ID};//用于向上位机上传小车状态信息的缓存
u8	WINCCMsgCache[160];//存储空间，用于存储所有可以存储的消息
u8	TempCache[15];//缓存，用于缓存刚刚接收到的消息
u8	uWINCCCmd[6];//用于给Function获取命令的缓存
u16	WINCCMsgCacheIndex	= 0x00;//缓存空间存放位置标志
u16 sTemp;//用于将yaw转换为2字节命令的缓存
u8	LatestMsgSaveIndex	= 0x10;//标记最后一次消息存储的位置
u8	LatestMsgGetIndex	= 0x10;//标记最后一次消息读取的位置
int iRealAx = 0;//实时坐标变量
int iRealAy = 0;
void ConnectToWINCC()
{
	while(1)
	{
		delay_ms(1000);
		if(!IsCarRegistered)
			COM_SendMessageToWINCC(3, ApplyToRegister);
		else
			break;
	}
}

void CheckMsgBuffStatus()//此函数由软件中断执行，优先级高于主函数，低于串口
{
	u8	Temp, TempEx;
	for(Temp = 1; Temp < 9; Temp++)
	{
		TempCache[Temp] = uBuffer[Temp];
	}
	//打开WINCC串口接收功能
	USART1->CR1 |= 0x04;

	Temp	= TempCache[2] + TempCache[3] + TempCache[4] + TempCache[5] + TempCache[6] + TempCache[7];
	if(Temp == uBuffer[8])//验证命令有效
	{
		if(0x10 == TempCache[2])//此命令为定位数据，直接更新定数数据，不作保存处理
		{
			iRealAx = TempCache[3];
			iRealAx = iRealAx << 8;
			iRealAx += TempCache[4];
			iRealAy = TempCache[5];
			iRealAy = iRealAy << 8;
			iRealAy += TempCache[6];
		}
		else if(0x13 == TempCache[2])//紧急停止，此函数一旦被调用后不再继续执行自动运行内容，且不可被恢复
		{
			EmergencyStop = true;
		}
		else if(WINCCMsgCacheIndex != 0xFFFF)//缓存空间尚未达到最大，可以继续保存
		{
			FeedBackToWINCC();
			if(0x10 == LatestMsgSaveIndex)//更新最新的存储位置
				LatestMsgSaveIndex = 0x00;
			else
				LatestMsgSaveIndex++;
			
			WINCCMsgCacheIndex |= 0x01 << LatestMsgSaveIndex;//记录存储的索引位置
			for(Temp = LatestMsgSaveIndex * 10, TempEx = 0; TempEx < 10; Temp++, TempEx++)//保存Msg
			{
				WINCCMsgCache[Temp] = uBuffer[TempEx];
			}
		}
		else
		{
			//关闭WINCC串口接收功能，缓存达到最大值，不再缓存
			USART1->CR1 &= ~0x04;
		}
	}
	else//验证命令无效
	{
		COM_SendMessageToWINCC(3, ErrorDuringTransfer);
	}
	bWINCCCmdIsBusy	= FALSE;

}

bool TryToGetMsgFromWINCC()
{
	u8 Temp, TempEx;
	if(0 == WINCCMsgCacheIndex)//缓存区不存在数据
		return false;
	else
	{
		if(0x10 == LatestMsgGetIndex)//更新最新的获取命令串位置
			LatestMsgGetIndex = 0;
		else
			LatestMsgGetIndex++;
		
		//获取内容
		WINCCMsgCacheIndex &= ~(0x01 << LatestMsgGetIndex);//修改存储的索引位置
		for(Temp = LatestMsgGetIndex * 10 + 2, TempEx = 0; TempEx < 6; Temp++, TempEx++)
		{
			uWINCCCmd[TempEx] = WINCCMsgCache[Temp];//拷贝数据至Function层，共6个字节
		}
	}
	return true;
}

void EXTI4_IRQHandler(void)
{
	EXTI->PR = 0x10;//清除中断标志
	//关闭WINCC串口接收功能
	USART1->CR1 &= ~0x04;
	
	CheckMsgBuffStatus();
}


void SendCarRunStatusToWINCC()
{
	CarInfoBuffer[2] = 0x10;
	CarInfoBuffer[3] = GetWheelSpeed(LEFT_FRONT);
	CarInfoBuffer[4] = GetWheelSpeed(LEFT_BACK);
	CarInfoBuffer[5] = GetWheelSpeed(RIGHT_FRONT);
	CarInfoBuffer[6] = GetWheelSpeed(RIGHT_BACK);
	CarInfoBuffer[7] = GetWheelDirection();
	CarInfoBuffer[8] = CarInfoBuffer[2] + CarInfoBuffer[3] + CarInfoBuffer[4] + CarInfoBuffer[5] + 
						CarInfoBuffer[6] + CarInfoBuffer[7];
	CarInfoBuffer[9] = 0xAA;
	COM_SendMessageToWINCC(10, CarInfoBuffer);
}

void SendCarTaskStatusToWINCC(float yaw, u8 TaskCount, u8 TaskIndex)
{
	sTemp = (u16)(yaw * 100);
	
	CarInfoBuffer[2] = 0x20;
	CarInfoBuffer[3] = (u8)(sTemp >> 8);
	CarInfoBuffer[4] = (u8)(sTemp);
	CarInfoBuffer[5] = TaskCount;
	CarInfoBuffer[6] = TaskIndex;
	CarInfoBuffer[7] = 0;
	CarInfoBuffer[8] = CarInfoBuffer[2] + CarInfoBuffer[3] + CarInfoBuffer[4] + CarInfoBuffer[5] + 
						CarInfoBuffer[6] + CarInfoBuffer[7];
	CarInfoBuffer[9] = 0xAA;
	COM_SendMessageToWINCC(10, CarInfoBuffer);
}

void SendCarYawAndSpeedToWINCC(float yaw)
{	
	sTemp = (u16)(yaw * 100);
	CarInfoBuffer[2] = 0x25;
	CarInfoBuffer[3] = (u8)(sTemp >> 8);
	CarInfoBuffer[4] = (u8)(sTemp);
	
	CarInfoBuffer[5] = GetWheelSpeed(LEFT_FRONT);
	CarInfoBuffer[6] = GetWheelSpeed(RIGHT_FRONT);
	CarInfoBuffer[7] = GetWheelDirection();
	CarInfoBuffer[8] = CarInfoBuffer[2] + CarInfoBuffer[3] + CarInfoBuffer[4] + CarInfoBuffer[5] + 
						CarInfoBuffer[6] + CarInfoBuffer[7];
	CarInfoBuffer[9] = 0xAA;
	COM_SendMessageToWINCC(10, CarInfoBuffer);
}

void SendFinishSingleTask(u16 deviation)
{
	CarInfoBuffer[2] = 0x26;
	CarInfoBuffer[3] = (u8)(deviation >> 8);
	CarInfoBuffer[4] = (u8)(deviation);
	CarInfoBuffer[5] = 0x01;
	CarInfoBuffer[6] = 0x00;
	CarInfoBuffer[7] = 0x00;
	CarInfoBuffer[8] = CarInfoBuffer[2] + CarInfoBuffer[3] + CarInfoBuffer[4] + CarInfoBuffer[5] + 
						CarInfoBuffer[6] + CarInfoBuffer[7];
	CarInfoBuffer[9] = 0xAA;
	COM_SendMessageToWINCC(10, CarInfoBuffer);
}


///////////////////////////////////////////  以下函数为监控使用，正式版不搭载   ////////////////////////////////////////////////
void SuperviseCurrentYaw(float yaw)
{
	sTemp = (u16)(yaw * 100);
	CarInfoBuffer[2] = 0xA0;
	CarInfoBuffer[3] = (u8)(sTemp >> 8);
	CarInfoBuffer[4] = (u8)(sTemp);
	CarInfoBuffer[5] = 0x00;
	CarInfoBuffer[6] = 0x00;
	CarInfoBuffer[7] = 0x00;
	CarInfoBuffer[8] = CarInfoBuffer[2] + CarInfoBuffer[3] + CarInfoBuffer[4] + CarInfoBuffer[5] + 
						CarInfoBuffer[6] + CarInfoBuffer[7];
	CarInfoBuffer[9] = 0xAA;
	COM_SendMessageToWINCC(10, CarInfoBuffer);
}

void SuperviseAllTaskCount(u8 count)
{
	CarInfoBuffer[2] = 0xA1;
	CarInfoBuffer[3] = count;
	CarInfoBuffer[4] = 0x00;
	CarInfoBuffer[5] = 0x00;
	CarInfoBuffer[6] = 0x00;
	CarInfoBuffer[7] = 0x00;
	CarInfoBuffer[8] = CarInfoBuffer[2] + CarInfoBuffer[3] + CarInfoBuffer[4] + CarInfoBuffer[5] + 
						CarInfoBuffer[6] + CarInfoBuffer[7];
	CarInfoBuffer[9] = 0xAA;
	COM_SendMessageToWINCC(10, CarInfoBuffer);
}


void SuperviseTaskDetail(u8 index, u16 x, u16 y)
{
	CarInfoBuffer[2] = 0xA2;
	CarInfoBuffer[3] = index;
	CarInfoBuffer[4] = (u8)(x >> 8);
	CarInfoBuffer[5] = (u8)(x);
	CarInfoBuffer[6] = (u8)(y >> 8);
	CarInfoBuffer[7] = (u8)(y);
	CarInfoBuffer[8] = CarInfoBuffer[2] + CarInfoBuffer[3] + CarInfoBuffer[4] + CarInfoBuffer[5] + 
						CarInfoBuffer[6] + CarInfoBuffer[7];
	CarInfoBuffer[9] = 0xAA;
	COM_SendMessageToWINCC(10, CarInfoBuffer);
}

void SuperviseTaskIndexAndTargetPos(u8 index, u16 x, u16 y)
{
	CarInfoBuffer[2] = 0xA3;
	CarInfoBuffer[3] = index;
	CarInfoBuffer[4] = (u8)(x >> 8);
	CarInfoBuffer[5] = (u8)(x);
	CarInfoBuffer[6] = (u8)(y >> 8);
	CarInfoBuffer[7] = (u8)(y);
	CarInfoBuffer[8] = CarInfoBuffer[2] + CarInfoBuffer[3] + CarInfoBuffer[4] + CarInfoBuffer[5] + 
						CarInfoBuffer[6] + CarInfoBuffer[7];
	CarInfoBuffer[9] = 0xAA;
	COM_SendMessageToWINCC(10, CarInfoBuffer);
}

void SuperviseTargetAndPresentYaw(float TargetYaw, float CurrentYaw)
{
	CarInfoBuffer[2] = 0xA4;
	sTemp = (u16)(TargetYaw * 100);
	CarInfoBuffer[3] = (u8)(sTemp >> 8);
	CarInfoBuffer[4] = (u8)(sTemp);
	sTemp = (u16)(CurrentYaw * 100);
	CarInfoBuffer[5] = (u8)(sTemp >> 8);
	CarInfoBuffer[6] = (u8)(sTemp);
	CarInfoBuffer[7] = 0x00;
	CarInfoBuffer[8] = CarInfoBuffer[2] + CarInfoBuffer[3] + CarInfoBuffer[4] + CarInfoBuffer[5] + 
						CarInfoBuffer[6] + CarInfoBuffer[7];
	CarInfoBuffer[9] = 0xAA;
	COM_SendMessageToWINCC(10, CarInfoBuffer);
}

void SuperviseTaskFinish(u16 error)
{
	CarInfoBuffer[2] = 0xA5;
	CarInfoBuffer[3] = (u8)(error >> 8);
	CarInfoBuffer[4] = (u8)(error);
	CarInfoBuffer[5] = 0x00;
	CarInfoBuffer[6] = 0x00;
	CarInfoBuffer[7] = 0x00;
	CarInfoBuffer[8] = CarInfoBuffer[2] + CarInfoBuffer[3] + CarInfoBuffer[4] + CarInfoBuffer[5] + 
						CarInfoBuffer[6] + CarInfoBuffer[7];
	CarInfoBuffer[9] = 0xAA;
	COM_SendMessageToWINCC(10, CarInfoBuffer);
}






