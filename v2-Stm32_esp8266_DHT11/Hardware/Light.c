#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Light.h"

void Light_Init(void)
{
	//使用GPIOA的第0引脚
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
}

u8 Light_GetNum(void)
{
	uint8_t State = 0;
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)
	{
		State = 1;
	}
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 1)
	{
		State = 0;
	}
	return State;
}


