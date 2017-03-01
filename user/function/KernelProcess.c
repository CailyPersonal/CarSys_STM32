///////////////////////////// start of file ///////////////////////////////////
/******************************************************************************
Module:  KernelProcess.h
Notices: 1.Copyright (c) 2015 CailyPersonal 
******************************************************************************/

//Include the standard Windows C++ and C-Runtime header files here.
#include "stm32f10x.h"
#include "KernelProcess.h"
#include "PublicDataCtrl.h"
#include "Functions.h"
#include "math.h"
#include "WirelessConnect.h"
#include "DirectionModel.h"
#include "CarRunCtrl.h"
#include "Time.h"
extern struct sTask Tasks[];
extern u8	uWINCCCmd[6];
extern int iRealAx;
extern int iRealAy;

bool bReachTurnDegree;
bool EmergencyStop;
///////////////////////////////////////////////////////////////////////////////

//����Ŀ��Ƕ�
float CalculateDegree(int iTx, int iTy)
{
	float fTDegree,fX,fY;								//								  |
	if(iTx > iRealAx)//��2��3����						//								  |
	{													//		<---------------X---------|-------
		if(iTy > iRealAy )//�ڵ�3����					//								  |	
		{												//								  |
			fX = iTx - iRealAx;							//		   2  |	 1				  Y
			fY = iTy - iRealAy;							//		------|-------			  |
			fTDegree = atan(fX/fY)*180/3.1415; 			//		   3  |	 4				  |
		}												//			  |					  |
		else//�ڵ�2����									//			 \|/ 0��				 \|/	
		{								
			fX = iTx - iRealAx;
			fY = iRealAy - iTy;
			fTDegree = 90 + atan(fY/fX)*180/3.1415;
		}
	}
	else//��1��4����
	{
		if(iTy > iRealAy)//�ڵ�4����
		{
			fX = iRealAx - iTx;
			fY = iTy - iRealAy;
			fTDegree = 270 + atan(fY/fX)*180/3.1415;
		}
		else//�ڵ�1����
		{
			fX = iRealAx - iTx;
			fY = iRealAy - iTy;
			fTDegree = atan(fX/fY)*180/3.1415 + 180;
		}
	}
	return fTDegree;
}

float CalculateDegreeDeviation(float fCrDegree, float fTgDegree){
	float fReturn;
	fReturn = fTgDegree - fCrDegree;
	
	/* Correct degree, this is necessary*/
	if(fReturn > 180)
		fReturn -= 360;
	if(fReturn < -180)
		fReturn += 360;
	return fReturn;
}


/************************* LineAmend *************************
Name  : LineAmend
Param : None
Notice: None
**************************************************************/
u8 LineAmend()
{
	u8 uCount, uTemp;
	u16 uBufferX, uBufferY, utBuffer, x, y;
	float fCrDegree, fTgDegree;
	
	EmergencyStop = false;
	
	while(0 == iRealAx && 0 == iRealAy);//��δ�յ���λ��Ϣ���ȵȴ�	
	uCount = 0;
	for(; uCount < 16; )
	{
		x = Tasks[uCount].iAx;
		y = Tasks[uCount].iAy;
		SuperviseTaskIndexAndTargetPos(uCount, x, y);//------------------------------
		
		fTgDegree = CalculateDegree(x, y);//����Ŀ��λ�ýǶ�
		fCrDegree = GetCurrentDirection();//��ȡ��ǰ�Ƕ�
		
		SuperviseTargetAndPresentYaw(fTgDegree, fCrDegree);//------------------------
		
		bReachTurnDegree = FALSE;//����Ƕȱ�־
		SetDegreeAlarm(fTgDegree);//����Э������֪ͨ�Ƕ�
		//�ж���ת����
		fCrDegree = CalculateDegreeDeviation(fCrDegree, fTgDegree);
		if(fCrDegree < 0)
			CarTurnLeft(0x80);
		else
			CarTurnRight(0x80);
		//����Ŀ��Ƕȣ�ֹͣ��ת
		while(!bReachTurnDegree);
		CarStopTurn(0x00);
		delay_ms(1000);
		
		CarForward(0xAA);
		fCrDegree = GetCurrentDirection();//��ȡ��ǰ�Ƕ�
		SuperviseCurrentYaw(fCrDegree);//--------------------------------------------
		
		while(1)
		{
			if(TRUE == EmergencyStop)//�������˽���ֹͣ��ť��ֱ���˳�����
			{
				CarStop();
				return 0x00;
			}
			uBufferX = x - iRealAx;
			uBufferY = y - iRealAy;
			utBuffer = uBufferX*uBufferX + uBufferY*uBufferY;
			if(utBuffer < 900)//Go to the range of the target
			{
				CarStop();
				Tasks[uCount].bIsDone = true;
				break;//Finish this task of target, go to next target
			}
			else
			{
				/* Process one: calculate the degree */
				fTgDegree = CalculateDegree(x, y);
				
				/* Process two: apply for a degree */
				fCrDegree = GetCurrentDirection();
				
				SuperviseTargetAndPresentYaw(fTgDegree, fCrDegree);//---------------------
				
				/* Process three: check if the deviation is allowed */
				fCrDegree = CalculateDegreeDeviation(fCrDegree, fTgDegree);
				
				if(fCrDegree < -3 || fCrDegree > 3)
				{				
					/* Process four: modify degree */
					if(fCrDegree > 0)
					{
						if(fCrDegree > 255)
							uTemp = 255;
						else
							uTemp = fCrDegree;
						CarRightAdjust(uTemp * 4);
					}
					else
					{
						fCrDegree = - fCrDegree;
						if(fCrDegree > 255)
							uTemp = 255;
						else
							uTemp = fCrDegree;
						CarLeftAdjust(uTemp * 4);
					}
				}//End if	
			}
		}
		if(true == Tasks[uCount].bIsContinue)
		{
			uCount++;
		}
		else
		{
			SuperviseTaskFinish(utBuffer);//-----------------------------------------------
			//SendFinishSingleTask(utBuffer);
			return 1;
		}
	}//End for
	return 0;
}


void HandleControl(u16 x, u16 y)
{
	float fCrDegree, fTgDegree;
	u16 uBufferX, uBufferY, utBuffer;
	u8 uTemp;
	EmergencyStop = false;
	while(0 == iRealAx && 0 == iRealAy);//��δ�յ���λ��Ϣ���ȵȴ�
	SuperviseTaskIndexAndTargetPos(0, x, y);
	
	
	fTgDegree = CalculateDegree(x, y);//����Ŀ��λ�ýǶ�
	
	fCrDegree = GetCurrentDirection();//��ȡ��ǰ�Ƕ�
	
	SuperviseTargetAndPresentYaw(fTgDegree, fCrDegree);
	
	bReachTurnDegree = FALSE;//����Ƕȱ�־
	SetDegreeAlarm(fTgDegree);//����Э������֪ͨ�Ƕ�
	//�ж���ת����
	fCrDegree = CalculateDegreeDeviation(fCrDegree, fTgDegree);
	if(fCrDegree < 0)
		CarTurnLeft(0x80);
	else
		CarTurnRight(0x80);
	//����Ŀ��Ƕȣ�ֹͣ��ת
	while(!bReachTurnDegree);
	CarStopTurn(0x00);
	delay_ms(1000);
	
	CarForward(0xAA);
	fCrDegree = GetCurrentDirection();//��ȡ��ǰ�Ƕ�
	SuperviseCurrentYaw(fCrDegree);

	while(1)
	{
		if(TRUE == EmergencyStop)//�������˽���ֹͣ��ť��ֱ���˳�����
		{
			CarStop();
			break;
		}
		uBufferX = x - iRealAx;
		uBufferY = y - iRealAy;
		utBuffer = uBufferX*uBufferX + uBufferY*uBufferY;
		if(utBuffer < 625)
		{
			SendFinishSingleTask(utBuffer);
			CarStop();
			SuperviseTaskFinish(utBuffer);
			
			return;
		}
		else
		{
			/* Process one: calculate the degree */
			fTgDegree = CalculateDegree(x, y);
			
			/* Process two: apply for a degree */
			fCrDegree = GetCurrentDirection();
			
			SuperviseTargetAndPresentYaw(fTgDegree, fCrDegree);
			
			/* Process three: check if the deviation is allowed */
			fCrDegree = CalculateDegreeDeviation(fCrDegree, fTgDegree);
			
			SuperviseCurrentYaw(fCrDegree);
			
			
			if(fCrDegree < -3 || fCrDegree > 3)
			{				
				/* Process four: modify degree */
				if(fCrDegree > 0)
				{
					if(fCrDegree > 255)
						uTemp = 255;
					else
						uTemp = fCrDegree;
					CarRightAdjust(uTemp * 4);
				}
				else
				{
					fCrDegree = - fCrDegree;
					if(fCrDegree > 255)
						uTemp = 255;
					else
						uTemp = fCrDegree;
					CarLeftAdjust(uTemp * 4);
				}
			}//End if	
		}
	}
}



/************************** Invsqrt **************************
Name  : Invsqrt
Param : uMath
Notice: 1.This is used to get the square root
		2.This algorithm comes from "Carmack " 
		3.http://www.matrix67.com/data/InvSqrt.pdf
**************************************************************/
float Invsqrt(float uMath){
	float xhalf = 0.5f * uMath;
	int i = *(int*)&uMath; // get bits for floating VALUE 
	i = 0x5f375a86- (i>>1); // gives initial guess y0
	uMath = *(float*)&i; // convert bits BACK to float
	uMath = uMath*(1.5f-xhalf*uMath*uMath); // Newton step, repeating increases accuracy
	return uMath;
}

void EXTI0_IRQHandler()
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		bReachTurnDegree = TRUE;
		/* Clear the  EXTI line 0 pending bit */
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

////////////////////////////  end of file  ////////////////////////////////////
