#include "Time.h"
#include "stm32f10x.h"
void delay_ms(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
	  i=12000;  
	  while(i--) ;    
   }
}

void InitTime()
{
	EXTI->IMR |= 0x01;
	EXTI->EMR |= 0x01;//打开1通道的请求
}



