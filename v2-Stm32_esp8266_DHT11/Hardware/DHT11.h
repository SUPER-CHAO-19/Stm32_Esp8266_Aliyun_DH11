#ifndef __DHT11_H
#define __DHT11_H

// 定义DHT11的数据引脚
#define DHT11_RCC       RCC_APB2Periph_GPIOB
#define DHT11_IO        GPIOB
#define DHT11_PIN       GPIO_Pin_15

// 函数声明
u8 DHT11_Init(void);       // 初始化DHT11
u8 DHT11_Read_Data(float *temp, float *humi);   // 读取DHT11数据
// u8 DHT11_Read_Data(u8 *temp, u8 *humi);   // 读取DHT11数据


#endif



