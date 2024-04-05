#include "stm32f10x.h"                 // Device header
#include "Delay.h"                     // 时间延时函数
#include "DHT11.h"                     // DHT11头文件
#include <string.h>


static void GPIO_SETOUT(void);          // 定义一个静态函数GPIO_SETOUT 作用是设置为输出模式
static void DHT11_Rst(void);            // 定义一个静态函数DHT11_Rst 作用是主机发送开始信号
static void GPIO_SETIN(void);           // 定义一个静态函数GPIO_SETIN 作用是设置为输入模式
static u8 DHT11_Check(void);            // 定义一个静态函数DHT11_Check 作用是检测DHT11的存在


/******************
 * 函数名称：DHT11_SETOUT
 * 函数功能：设置为输出模式
 * 输入参数：无
 * 输出参数：无
 * 调用示例：DHT11_SETOUT();
*******************/
static void GPIO_SETOUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;   // 定义一个GPIO_InitTypeDef类型的结构体变量GPIO_InitStructure
    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;     // 设置DHT11的IO口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 设置为推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 设置IO口速度为10MHz
    GPIO_Init(DHT11_IO, &GPIO_InitStructure);         // 初始化DHT11的IO口
}

/******************
 * 函数名称：DHT11_SETIN
 * 函数功能：设置为输入模式
 * 输入参数：无
 * 输出参数：无
 * 调用示例：DHT11_SETIN();
*******************/
static void GPIO_SETIN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;   // 定义一个GPIO_InitTypeDef类型的结构体变量GPIO_InitStructure
    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;     // 设置DHT11的IO口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;     // 设置为浮空输入
    GPIO_Init(DHT11_IO, &GPIO_InitStructure);                 // 初始化DHT11的IO口
}

/******************
 * 函数名称：DHT11_Rst
 * 函数功能：主机发送开始信号
 * 输入参数：无
 * 输出参数：无
 * 调用示例：DHT11_Rst();
 * ****************/
static void DHT11_Rst(void)   // static 说明作用域在DHT11.c文件内
{
    GPIO_SETOUT();             // 设置为输出模式
    // 主机拉低电平20ms
    GPIO_ResetBits(DHT11_IO, DHT11_PIN);  // 主机拉低DHT11的IO口
    Delay_ms(20);              // 延时20ms
    // 主机拉高电平30us
    GPIO_SetBits(DHT11_IO, DHT11_PIN);    // 主机拉高DHT11的IO口
    Delay_us(30);              // 延时30us
}



/******************
 * 函数名称：DHT11_Check
 * 函数功能：检测DHT11的存在
 * 输入参数：无
 * 输出参数：u8 返回值1表示存在，0表示不存在
 * 调用示例：DHT11_Check();
******************/
static u8 DHT11_Check(void)
{
    u8 retry = 0;  // 定义一个重试次数
    GPIO_SETIN();  // 设置为输入模式
    // 首先DHT11拉低电平80us 
    // 这里检测如果一直是高电平且超过100us 判定为没有响应
    while ((GPIO_ReadInputDataBit(DHT11_IO, DHT11_PIN) == 1) && retry < 100)  // 当DHT11的IO口为高电平时 且 重试次数小于100
    {
        retry++;          // 重试次数加1
        Delay_us(1);      // 延时1us
    }

    if (retry >= 100)     // 如果重试次数大于等于100
    {
        return 0;         // 返回0
    }
    
    else
    {
        retry = 0;        // 重试次数清零
    }
    // 接着DHT11拉高电平80us 
    // 这里检测如果一直是低电平且超过100us 判定为没有响应
    while ((GPIO_ReadInputDataBit(DHT11_IO, DHT11_PIN) == 0) && retry < 100)  // 当DHT11的IO口为低电平时 且 重试次数小于100
    {
        retry++;      // 重试次数加1
        Delay_us(1);  // 延时1us
    }

    if (retry >= 100) // 如果重试次数大于等于100
    {
        return 0;     // 返回0
    }
    
    return 1;  // 返回1
}

/******************
 * 函数名称：DHT11_Init
 * 函数功能：初始化DHT11
 * 输入参数：无
 * 输出参数：u8 返回值1表示初始化成功，0表示初始化失败
 * 调用示例：DHT11_Init();
*******************/
u8 DHT11_Init(void)
{
    RCC_APB2PeriphClockCmd(DHT11_RCC, ENABLE);  // 使能DHT11的时钟

    DHT11_Rst();               // 主机发送开始信号

    return DHT11_Check();      // 返回检测结果
}



/******************
 * 函数名称：static u8 DHT11_ReadBit(void)
 * 函数功能：读取一个位
 * 输入参数：无
 * 输出参数：u8 返回值1表示读取到1，0表示读取到0
 * 调用示例：DHT11_ReadBit();
******************/
static u8 DHT11_ReadBit(void)
{
    u8 retry = 0;          // 定义一个重试次数

    // DHT11拉低电平50us 
    // 这里检测如果一直是高电平且超过100us 退出循环
    while ((GPIO_ReadInputDataBit(DHT11_IO, DHT11_PIN) == 1) && retry < 100)  // 当DHT11的IO口为高电平时 且 重试次数小于100
    {
        retry++;           // 重试次数加1
        Delay_us(1);      // 延时1us
    }

    retry = 0;          // 重试次数清零

    // 接着DHT11拉高电平26-28us或者70us 
    // 这里检测如果一直是低电平且超过100us 退出循环
    while ((GPIO_ReadInputDataBit(DHT11_IO, DHT11_PIN) == 0) && retry < 100)  // 当DHT11的IO口为低电平时 且 重试次数小于100
    {
        retry++;      // 重试次数加1
        Delay_us(1);  // 延时1us
    }

    // 0信号为26-28us，1信号则为70us 相当于高电平的第40us时候的判断电平所处状态
    Delay_us(40);    // 延时40us
    if ((GPIO_ReadInputDataBit(DHT11_IO, DHT11_PIN) == 1))  // 如果DHT11的IO口为高电平
    {
        return 1;  // 返回1
    }
    else
    {
        return 0;  // 返回0
    }
}

/******************
 * 函数名称：static u8 DHT11_ReadByte(void)
 * 函数功能：读取一个字节
 * 输入参数：无
 * 输出参数：u8 返回值为读取到的字节
 * 调用示例：DHT11_ReadByte();
*******************/
static u8 DHT11_ReadByte(void)
{
    u8 i, dat = 0;  // 定义一个变量i和dat

    for (i = 0; i < 8; i++)  // 循环8次
    {
        dat <<= 1;           // dat左移1位
        dat |= DHT11_ReadBit();  // dat或上读取到的位
 }
    return dat;  // 返回dat
}

/******************
 * 函数名称：DHT11_Read_Data
 * 函数功能：读取DHT11的数据
 * 输入参数：float *temp 温度指针，float *humi 湿度指针
 * 输出参数：u8 返回值1表示读取成功，0表示读取失败
 * 调用示例：DHT11_ReadData(&temp,&humi);如何调用
*******************/

u8 DHT11_Read_Data(float *temp, float *humi)
{
    u8 buf[5];  // 定义一个长度为5的数组buf
    memset(buf, 0, sizeof(buf));
    u8 i;       // 定义一个变量i

    DHT11_Rst();  // 主机发送开始信号
    if (DHT11_Check() == 1)  // 如果检测DHT11存在
    {
        for (i = 0; i < 5; i++)  // 循环5次
        {
            buf[i] = DHT11_ReadByte();  // 读取一个字节
        }

        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])  // 如果前4个字节的和等于第5个字节 说明数据正确
        {
            *humi = buf[0] + buf[1] / 10.0;  // 湿度等于第一个字节加上第二个字节除以10 
            *temp = buf[2] + buf[3] / 10.0;  // 温度等于第三个字节加上第四个字节除以10
            // *humi = buf[0];				//将湿度值放入指针humi
            // *temp = buf[2];				//将温度值放入指针temp
            memset(buf, 0, sizeof(buf));      // 清空buf
            return 1;  // 返回1
        }
    }
    else
    {
        memset(buf, 0, sizeof(buf));  // 清空buf
        return 0;  // 返回0
    }
}


