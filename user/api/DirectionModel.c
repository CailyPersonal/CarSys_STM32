#include "DirectionModel.h"
#include "COMCommunicate.h"
#include "Time.h"
#include "stm32f10x.h"

u8 uGetDegreeBuffer[5]		= {0xA5, 0x5A, 0x01, 0x00, 0x00};
u8 uSetZeroDegree[5]		= {0xA5, 0x5A, 0x00, 0x00, 0x00};
u8 uSetTgDegreeBuffer[5]	= {0xA5, 0x5A, 0x02, 0x00, 0x00};
extern u8 uOuterDegreeBuffer[2];
extern bool bGetDirection;
void SetZeroDegree()
{
	COM_SendMessageToOuterDevice(5, uSetZeroDegree);
}

void SetDegreeAlarm(float degree)
{
	u16 sTemp;
	sTemp = (u16)(degree * 100);
	uSetTgDegreeBuffer[3] = (u8)(sTemp >> 8);
	uSetTgDegreeBuffer[4] = (u8)(sTemp);
	COM_SendMessageToOuterDevice(5, uSetTgDegreeBuffer);
}

float GetCurrentDirection()
{
	float direction;
	u16 temp;
	bGetDirection = false;
	COM_SendMessageToOuterDevice(5, uGetDegreeBuffer);
	while(!bGetDirection)
	{
		temp++;
		delay_ms(10);
		if(10 == temp)
		{
			COM_SendMessageToOuterDevice(5, uGetDegreeBuffer);
			temp = 0;
		}
	}
	temp = uOuterDegreeBuffer[0];
	temp = temp << 8;
	temp += uOuterDegreeBuffer[1];
	direction = temp;
	direction /= 100;

	return direction; 
}















