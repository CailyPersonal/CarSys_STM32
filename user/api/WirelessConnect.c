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
u8	ApplyToRegister[3]			= {0xA5, CAR_BOARD_ID, 0xFE};//����ע�᱾С��
u8	ErrorDuringTransfer[3]		= {0xA5, CAR_BOARD_ID, 0x30};//֪ͨ��λ��ǰ�η�����Ϣ����
u8	CarInfoBuffer[10] = {0xA5, CAR_BOARD_ID};//��������λ���ϴ�С��״̬��Ϣ�Ļ���
u8	WINCCMsgCache[160];//�洢�ռ䣬���ڴ洢���п��Դ洢����Ϣ
u8	TempCache[15];//���棬���ڻ���ոս��յ�����Ϣ
u8	uWINCCCmd[6];//���ڸ�Function��ȡ����Ļ���
u16	WINCCMsgCacheIndex	= 0x00;//����ռ���λ�ñ�־
u16 sTemp;//���ڽ�yawת��Ϊ2�ֽ�����Ļ���
u8	LatestMsgSaveIndex	= 0x10;//������һ����Ϣ�洢��λ��
u8	LatestMsgGetIndex	= 0x10;//������һ����Ϣ��ȡ��λ��
int iRealAx = 0;//ʵʱ�������
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

void CheckMsgBuffStatus()//�˺���������ж�ִ�У����ȼ����������������ڴ���
{
	u8	Temp, TempEx;
	for(Temp = 1; Temp < 9; Temp++)
	{
		TempCache[Temp] = uBuffer[Temp];
	}
	//��WINCC���ڽ��չ���
	USART1->CR1 |= 0x04;

	Temp	= TempCache[2] + TempCache[3] + TempCache[4] + TempCache[5] + TempCache[6] + TempCache[7];
	if(Temp == uBuffer[8])//��֤������Ч
	{
		if(0x10 == TempCache[2])//������Ϊ��λ���ݣ�ֱ�Ӹ��¶������ݣ��������洦��
		{
			iRealAx = TempCache[3];
			iRealAx = iRealAx << 8;
			iRealAx += TempCache[4];
			iRealAy = TempCache[5];
			iRealAy = iRealAy << 8;
			iRealAy += TempCache[6];
		}
		else if(0x13 == TempCache[2])//����ֹͣ���˺���һ�������ú��ټ���ִ���Զ��������ݣ��Ҳ��ɱ��ָ�
		{
			EmergencyStop = true;
		}
		else if(WINCCMsgCacheIndex != 0xFFFF)//����ռ���δ�ﵽ��󣬿��Լ�������
		{
			FeedBackToWINCC();
			if(0x10 == LatestMsgSaveIndex)//�������µĴ洢λ��
				LatestMsgSaveIndex = 0x00;
			else
				LatestMsgSaveIndex++;
			
			WINCCMsgCacheIndex |= 0x01 << LatestMsgSaveIndex;//��¼�洢������λ��
			for(Temp = LatestMsgSaveIndex * 10, TempEx = 0; TempEx < 10; Temp++, TempEx++)//����Msg
			{
				WINCCMsgCache[Temp] = uBuffer[TempEx];
			}
		}
		else
		{
			//�ر�WINCC���ڽ��չ��ܣ�����ﵽ���ֵ�����ٻ���
			USART1->CR1 &= ~0x04;
		}
	}
	else//��֤������Ч
	{
		COM_SendMessageToWINCC(3, ErrorDuringTransfer);
	}
	bWINCCCmdIsBusy	= FALSE;

}

bool TryToGetMsgFromWINCC()
{
	u8 Temp, TempEx;
	if(0 == WINCCMsgCacheIndex)//����������������
		return false;
	else
	{
		if(0x10 == LatestMsgGetIndex)//�������µĻ�ȡ���λ��
			LatestMsgGetIndex = 0;
		else
			LatestMsgGetIndex++;
		
		//��ȡ����
		WINCCMsgCacheIndex &= ~(0x01 << LatestMsgGetIndex);//�޸Ĵ洢������λ��
		for(Temp = LatestMsgGetIndex * 10 + 2, TempEx = 0; TempEx < 6; Temp++, TempEx++)
		{
			uWINCCCmd[TempEx] = WINCCMsgCache[Temp];//����������Function�㣬��6���ֽ�
		}
	}
	return true;
}

void EXTI4_IRQHandler(void)
{
	EXTI->PR = 0x10;//����жϱ�־
	//�ر�WINCC���ڽ��չ���
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


///////////////////////////////////////////  ���º���Ϊ���ʹ�ã���ʽ�治����   ////////////////////////////////////////////////
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






