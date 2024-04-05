#include "stm32f10x.h"                  // Device header

//TIM2

void Timer2_Init(void)
{
	//启用了TIM2定时器的时钟 TIM2是连接到APB1总线上的一个定时器。
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	//配置TIM2的时钟源为内部时钟 这意味着TIM2的时钟将由微控制器的内部时钟源提供。
	TIM_InternalClockConfig(TIM2);
	
	//定时器TIM2初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//配置时钟分频为1 即不分频。
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 10000-1; //10kHZ除以10000等于1s计时(计数9999次溢出实现定时1s)
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200-1;  //对72M进行7200分频,得到10kHZ (1/10000s计数一次)
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0; //参数用于高级计数器，这里设置为0
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	//用于清除TIM2的更新中断标志位 以确保在初始化后不会立即进入中断。
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	//启用TIM2的更新中断 当TIM2的计数器溢出时，将产生更新事件并触发中断。
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//配置中断优先级分组为Group 2。
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //指定中断通道为TIM2的通道。
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //启用中断通道。
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //配置中断抢占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         //配置中断子优先级为1
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM2, ENABLE);
}   


//TIM1
void Timer1_Init(void)
{
	//启用了TIM1定时器的时钟 TIM1是连接到APB2总线上的一个定时器。
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	//配置TIM1的时钟源为内部时钟 这意味着TIM1的时钟将由微控制器的内部时钟源提供。
	TIM_InternalClockConfig(TIM1);
	
	//定时器TIM1初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//配置时钟分频为1 即不分频。
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInitStructure.TIM_Period = 15000-1; //10kHZ除以10000等于1s计时(计数9999次溢出实现定时1s)
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200-1;  //对72M进行7200分频,得到10kHZ (1/10000s计数一次)
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0; //参数用于高级计数器，这里设置为0
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
	
	//用于清除TIM1的更新中断标志位 以确保在初始化后不会立即进入中断。
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	//启用TIM1的更新中断 当TIM2的计数器溢出时，将产生更新事件并触发中断。
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//配置中断优先级分组为Group 2。
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //指定中断通道为TIM1的通道。
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //启用中断通道。
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //配置中断抢占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;         //配置中断子优先级为1
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM1, DISABLE);
}   


//void TIM2_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
//	{
//		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
//	}	
//}
