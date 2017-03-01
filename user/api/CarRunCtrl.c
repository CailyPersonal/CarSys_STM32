

#include "CarRunCtrl.h"
#include "WheelCtrl.h"
#include "PublicDataCtrl.h"
#include "Time.h"

void CarStop()
{
	Wheel_SetRunSpeed(0x00);
	//�˴�Ӧ�ùر��жϿ���
}

void CarForward(u8 speed)
{
	Wheel_Direction_Forward();
	Wheel_SetRunSpeed(speed);
	//�˴�Ӧ�ô��жϿ���
}

void CarBackward(u8 speed)
{
	Wheel_Direction_Backward();
	Wheel_SetRunSpeed(speed);
}

void CarTurnLeft(u8 speed)
{
	Wheel_SimpleTurnToLeft();
	Wheel_SetRunSpeed(speed);
}

void CarTurnRight(u8 speed)
{
	Wheel_SimpleTurnToRight();
	Wheel_SetRunSpeed(speed);
}
	
void CarStopTurn(u8 speed)
{
	Wheel_SimpleStopTurn();
	Wheel_SetRunSpeed(speed);
}

void CarLeftAdjust(u8 time_ms)
{
	Wheel_SimpleTurnToLeft();
	delay_ms(time_ms);
	Wheel_SimpleStopTurn();
}

void CarRightAdjust(u8 time_ms)
{
	Wheel_SimpleTurnToRight();
	delay_ms(time_ms);
	Wheel_SimpleStopTurn();
}










