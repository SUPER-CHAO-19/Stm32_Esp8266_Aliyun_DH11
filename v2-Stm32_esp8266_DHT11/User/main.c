#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"
#include "Esp8266.h"
#include "DHT11.h"
#include "Timer.h"
#include "ALARM.h"
#include "Light.h"

u8 temp_humi_send_flag = 0;         //温湿度数据发送标志位
u8 count_flag = 0;                  //计数标志位

u8 hour = 0;                        //时钟
u8 minute = 0;                      //分钟
u8 second = 0;                      //秒钟



int main(void)
{
	OLED_Init();                       // OLED初始化

	Timer1_Init();                     // 定时器1初始化

	Serial_Init();                     // 串口1初始化
	// Serial2_Init();                    // 串口2初始化

	Alarm_Init();                      // 语音初始化
	Light_Init();                      // 光照传感器初始化
	DHT11_Init();                      // DHT11初始化

	OLED_ShowNum(1, 14, 0,  1);
	OLED_ShowString(1, 15, "/2");

	ESP8266_Init();                    // ESP8266初始化
	
	ESP8266_Time_Get(&hour, &minute, &second); //获取ESP8266的时间

	OLED_ShowNum(1, 14, 1,  1);
	OLED_ShowString(1, 15, "/2");

	Delay_s(1);

	ESP8266_Init();                            // ESP8266初始化 需初始化两次 否则时间不正确

	OLED_ShowNum(1, 14, 2,  1);
	OLED_ShowString(1, 15, "/2");

	ESP8266_Time_Get(&hour, &minute, &second); //获取ESP8266的时间

	



	Timer2_Init();                     // 定时器2初始化
	
	OLED_Clear();                      // OLED清屏

	// OLED_ShowString(1,1,"DHT11 Test");

	float temp, humi;                  //温湿度数据

	u8 light_state = 0;                  //光照强度

	u8 temp_set = 32;                  //温度报警阈值
	u8 humi_set = 78;                  //湿度报警阈值

	u8 hour_set = 0;                   //时钟报警阈值1
	u8 minute_set = 0;                 //时钟报警阈值1
	u8 second_set = 0;                 //时钟报警阈值1

	u8 hour_set2 = 0;                   //时钟报警阈值1
	u8 minute_set2 = 0;                 //时钟报警阈值1
	u8 second_set2 = 0;                 //时钟报警阈值1
	
	u8 temp_humi_flag = 0;             //温湿度报警标志位
	u8 time_flag = 0;                  //时钟报警标志位

	u8 s_d_flag = 0;

	u8 time_get_flag = 0;

	while(1)
	{	

		// 如果count_flag被设置，说明已经过了1s，此时将second加1
		if(count_flag == 1)
		{		
			time_get_flag = time_get_flag + 1;

			if (time_get_flag == 600)                 //如果time_get_flag等于600，说明已经过了10分钟，此时重新获取时间网络校准
			{
				//关闭定时器
				TIM_Cmd(TIM2, DISABLE);
				//重新获取时间
				ESP8266_Time_Get(&hour, &minute, &second);
				//打开定时器
				TIM_Cmd(TIM2, ENABLE);	
				time_get_flag = 0;
			}
			
			second = second + 1;
			if(second >= 60)       	 //如果second大于等于60，说明已经过了1分钟，此时将minute加1
			{
				second = 0;
				minute++;
			}
			if(minute >= 60)         //如果minute大于等于60，说明已经过了1小时，此时将hour加1
			{
				minute = 0;
				hour++;
			}
			if(hour >= 24)           //如果hour大于等于24，说明已经过了1天，此时将hour置0
			{
				hour = 0;
			}
			// OLED显示时间
			OLED_ShowString(1,1,"Time: ");
			OLED_ShowNum(1,7,hour, 2);
			OLED_ShowString(1,9,":");
			OLED_ShowNum(1,10,minute, 2);
			OLED_ShowString(1,12,":");
			OLED_ShowNum(1,13,second, 2);
			// 标志位清零
			count_flag = 0;
		}
		
		// 如果temp_humi_send_flag被设置，说明已经过了1s，此时读取温湿度数据
		if(temp_humi_send_flag == 1)
		{   
			// 读取温湿度数据
			if(DHT11_Read_Data(&temp, &humi) == 1)
			{
				static char temp_str[5];                       //温度字符串
				static char humi_str[5];                       //湿度字符串
				sprintf(temp_str, "Temp: %.1f C", temp);       //温度数据转化为字符串
				OLED_ShowString(2,1,temp_str);                 //在OLED上显示温度数据
				sprintf(humi_str, "Humi: %.1f %%", humi);      //湿度数据转化为字符串
				OLED_ShowString(3,1,humi_str);                 //在OLED上显示湿度数据

				// 在后面显示温湿度阈值
				OLED_ShowNum(2,14, temp_set, 2);
				OLED_ShowNum(3,14, humi_set, 2);					
			}
			// 如果DHT11读取数据失败
			else
			{
				OLED_ShowString(1,1,"DHT11 Error");             //在OLED上显示DHT11错误
			}

			light_state = Light_GetNum();                             //读取光照强度

			if (light_state == 1 )
			{
				time_flag = 0;                                          //说明打开过药箱
			}

			OLED_ShowNum(1, 16, light_state, 1);                      //在OLED上显示光照强度

			if(s_d_flag == 0)
			{
				ESP8266_Send("humidity", humi, "hour_set", hour_set, "minute_set", minute_set,"temp_set", temp_set, "hour_set2", hour_set2, "minute_set2", minute_set2);	 //发送温湿度数据到阿里云
				s_d_flag =1;
			}
			else
			{
				ESP8266_Send("temperature", temp, "switch", light_state, "humi_set",humi_set, "temp_humi_flag", temp_humi_flag, "time_flag", time_flag, "second_set", second_set);	 //发送温湿度数据到阿里云
				s_d_flag = 0;
			}
				
			
			temp_humi_send_flag = 0;                            //标志位清零
		}
		
		// 温度湿度报警功能
		if(temp > temp_set || humi > humi_set)  // 如果温度或湿度大于阈值,蜂鸣警报   
		{
			// Serial2_SendString("T_alarm\r\n");  // 串口2发送温度报警信息
			Alarm1_ON();                        // 蜂鸣警报
			temp_humi_flag = 1;                      // 温度报警标志位
		}
		else
		{
			Alarm1_OFF();                       // 关闭蜂鸣警报
			temp_humi_flag = 0;                      // 温度报警标志位
		}


		// 时钟报警功能
		if((hour == hour_set && minute == minute_set && second < 15) || (hour == hour_set2 && minute == minute_set2 && second < 15)) // 如果时间等于设置时间,语音提醒
		{
			// Serial2_SendString("H_alarm\r\n");   // 串口2发送时钟语音提醒
			Alarm2_ON();                         // 语音提醒
			time_flag = 1;                       // 时钟报警标志位 只有打开药箱 才能消除
		}
		else
		{
			Alarm2_OFF();                        // 语音提醒
		}


		// 判断串口1中断标志位是否被设置, 远程开关功能
		if(Serial_GetRxFlag() == 1)              
		{

			if(ESP8266_Received(&hour_set, &minute_set, &temp_set, &humi_set, &hour_set2, &minute_set2))    //接收阿里云发送的时间数据
			{
				ESP8266_Send("humidity", humi, "hour_set", hour_set, "minute_set", minute_set,"temp_set", temp_set, "hour_set2", hour_set2, "minute_set2", minute_set2);	 //发送温湿度数据到阿里云
				s_d_flag =0;
			}
		}

		// OLED显示设置的时间
		OLED_ShowString(4,1,"Hint: ");
		OLED_ShowNum(4,7,hour_set, 2);
		OLED_ShowString(4,9,":");
		OLED_ShowNum(4,10,minute_set, 2);
		// OLED_ShowString(4,12,"|");
		OLED_ShowNum(4,12,hour_set2, 2);
		OLED_ShowString(4,14,":");
		OLED_ShowNum(4,15,minute_set2, 2);

	}

}

// TIM2中断服务函数
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)          //检查TIM2的更新中断是否发生
	{
		temp_humi_send_flag = 1;                              //设置温湿度数据发送标志位
		
		count_flag = 1;                                       //设置计数变量

		// ESP8266_Send("humidity", humidity);                   //发送湿度数据       
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);           //清除TIM2的更新中断标志位
	}	
}


