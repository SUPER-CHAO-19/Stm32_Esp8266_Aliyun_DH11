#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "timer.h"

uint8_t Serial_RxData;
uint8_t Serial_RxFlag;


void Serial_Init(void)
{
	//开启USART1时钟 在APB2总线上
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	//开启GPIOA时钟 在APB2总线上
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	//配置A9为复用推挽输出 作为TX
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//配置A10为浮空输入 作为RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//浮空输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//配置USART1参数
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不使用流控
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx; //配置 USART 为同时支持发送和接收数据
	USART_InitStructure.USART_Parity = USART_Parity_No;             //不进行数据奇偶校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;          //一个停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;     //不用校验所以选择8位
	USART_Init(USART1, &USART_InitStructure);                       //初始化USART1
	
	//使能USART1接收中断 (USART_IT_RXNE 表示接收寄存器非空中断)。
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	////配置NVIC 配置中断优先级分组为Group 2。这是一种设置中断优先级的方式。
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;                     
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;        //指定中断通道为USART1串口中断
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			 //启动外部通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//配置中断抢占优先级为1。
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;       //配置中断子优先级为1。
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
}

// 串口2初始化
void Serial2_Init(void)
{
	// Enable USART2 clock on APB1 bus
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	// Enable GPIOA clock on APB2 bus
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	// Configure GPIOA pins for USART2
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // Alternate function push-pull
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;       // TX pin
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // Input floating
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;     // RX pin
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Configure USART2 parameters
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // No flow control
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;                // Enable both transmission and reception
	USART_InitStructure.USART_Parity = USART_Parity_No;                            // No parity check
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                         // 1 stop bit
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    // 8 data bits
	USART_Init(USART2, &USART_InitStructure);                                      // Initialize USART2

	// Enable USART2 receive interrupt (USART_IT_RXNE indicates non-empty receive register interrupt)
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	// Configure NVIC with interrupt priority group 2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;        // Specify USART2 interrupt channel
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          // Enable external channel
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // Configure preemption priority as 1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // Configure sub priority as 1
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USART2, ENABLE);
}


//串口1发送字
void Serial_SendByte(uint8_t Byte) 
{
	USART_SendData(USART1, Byte);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//1则清零完成
	
}

//串口2发送字
void Serial2_SendByte(uint8_t Byte) 
{
	USART_SendData(USART2, Byte);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

//串口1发送数组
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for(i = 0; i < Length; i++)
	{
		Serial_SendByte(Array[i]);
	}
}

//串口2发送数组
void Serial2_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for(i = 0; i < Length; i++)
	{
		Serial2_SendByte(Array[i]);
	}
}

//串口1发送字符串
void Serial_SendString(char *String)
{
	uint8_t i;
	for(i = 0; String[i] != '\0'; i++)//0->'\0'
	{
		Serial_SendByte(String[i]);
	}
}	

//串口2发送字符串
void Serial2_SendString(char *String)
{
	uint8_t i;
	for(i = 0; String[i] != '\0'; i++)
	{
		Serial2_SendByte(String[i]);
	}
}

//计算X的Y次方
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while(Y--)
	{
		Result *= X;
	}
	return Result;
}

//串口1发送数字字符
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for(i = 0; i<Length; i++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length-i-1)%10 + '0');
	}
}

//串口2发送数字字符
void Serial2_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for(i = 0; i<Length; i++)
	{
		Serial2_SendByte(Number / Serial_Pow(10, Length-i-1)%10 + '0');
	}
}

//重定向printf到串口
int fputc(int ch, FILE *f) 
{
	Serial_SendByte(ch);
	return ch;
}

//串口1打印
void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

void Serial2_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial2_SendString(String);
}

//用于检查串口接收标志位是否被设置。
//如果 Serial_RxFlag 为1（已经接收到数据），它将清零标志位并返回1；
//否则，返回0。
uint8_t Serial_GetRxFlag(void)
{
	if(Serial_RxFlag==1)
	{
		Serial_RxFlag=0;
		return 1;
	}
	return 0;
}

//用于获取串口接收到的数据。字节将被存储在 Serial_RxData 中。
uint8_t Serial_GetRxData(void)
{
	return Serial_RxData;
}

//一个串口接收字符串的函数，收到\r\n符号就停止接收，并返回接收的字符串。
//串口接收字符串
u8 time_out_rx = 0;

void Serial_ReceiveString(char *String)
{
	uint8_t i = 0;

	//启动定时器1
	TIM_Cmd(TIM1, ENABLE);
	//清除定时器1的更新中断标志位	
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);

	while(1)
	{
		
		while(Serial_GetRxFlag() == 0 && time_out_rx == 0);          //等待接收到数据
		if(time_out_rx == 1)                                        //如果串口接收超时
		{
			time_out_rx = 0;                                       //清除串口接收超时标志位
			TIM_Cmd(TIM1, DISABLE);                                //关闭定时器1
			break;                                              //返回0
		}
		String[i] = Serial_GetRxData();          //将接收到的数据存储在String中
		i++;
		if(String[i-1] == '\n' && String[i-2] == '\r')
		{
			String[i] = '\0';
			time_out_rx = 0;                                       //清除串口接收超时标志位
			TIM_Cmd(TIM1, DISABLE);                                //关闭定时器1
			break;
		}
	}
}
//如何调用这个函数？
// char String[100];
// Serial_ReceiveString(String);


//串口1中断处理函数,运行机制是串口接收到数据时，会触发接收中断
//当USART1串口接收到数据时，会触发接收中断。
void USART1_IRQHandler(void)
{
	if(USART_GetFlagStatus(USART1, USART_IT_RXNE) == SET)//检查接收中断标志位 收到数据 标志位为1
	{
		//如果接收到数据，它将数据存储在 Serial_RxData 中
		Serial_RxData = USART_ReceiveData(USART1);//数据转存
		//Serial_RxFlag 设置为1，表示已经接收到数据
		Serial_RxFlag = 1;
		//清除接收中断标志位。这一步是可选的，因为读取数据后，标志位会自动清零。
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

// //定时器1中断服务函数 用于串口接收超时
void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)          //检查TIM1的更新中断是否发生
	{
		
		time_out_rx = 1;                              //设置串口接收标志位 1s超时
		//清除TIM1的更新中断标志位
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}