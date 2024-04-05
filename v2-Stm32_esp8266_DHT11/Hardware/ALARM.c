#include "stm32f10x.h"                  // Device header

void Alarm_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;            
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_2|GPIO_Pin_3);
}

void Alarm1_ON(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_2);
	
}

void Alarm1_OFF(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);// 低电平触发
}

void Alarm1_Turn(void)
{
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_2);
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);
	}
}

void Alarm2_ON(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_3);
	
}

void Alarm2_OFF(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_3);
}

void Alarm2_Turn(void)
{
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3) == 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_3);
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_3);
	}
}

