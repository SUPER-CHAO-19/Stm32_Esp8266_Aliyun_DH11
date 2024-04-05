#include "stm32f10x.h"                  // Device header
#include "Serial.h"
#include "Esp8266.h"
#include "Delay.h"
#include "OLED.h"
#include <string.h>




/*************************
 * 函数名称：ESP8266_Init
 * 函数功能：初始化ESP8266
 * 输入参数：无
 * 输出参数：无
 * 调用示例：ESP8266_Init();
 **************************/
void ESP8266_Init(void)
{
	// 定义一个足够大的字符数组来存储合并后的字符串     
	char command[140];  
	memset(command, 0, sizeof(command));    //清空command数组

	//发送WIFI模式信息
	OLED_ShowString(3,1, "                 ");
	OLED_ShowString(3,1, "WIFI_MODE");
	sprintf(command, "AT+CWMODE=1\r\n");
	// Serial2_SendString(command);             //发送串口2给电脑看指令情况
	ESP8266_Init_Send(command, "OK1", "OR1");

	//发送WIFI连接信息
	OLED_ShowString(3,1, "                 ");
	OLED_ShowString(3,1, "WIFI_CONN");           
	sprintf(command, "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_Name, WIFI_Pass); 
	// Serial2_SendString(command);             //发送串口2给电脑看指令情况
	ESP8266_Init_Send(command, "OK2", "OR2");

	//发送MQTT连接用户信息
	OLED_ShowString(3,1, "                 ");
	OLED_ShowString(3,1, "MQTT_USER");  
	sprintf(command, "AT+MQTTUSERCFG=0,1,\"NULL\",\"%s\",\"%s\",0,0,\"\"\r\n",username,passwd);
	// Serial2_SendString(command);             //发送串口2给电脑看指令情况
	ESP8266_Init_Send(command, "OK3", "OR3");
	
	//发送MQTT连接ID信息
	OLED_ShowString(3,1, "                 ");
	OLED_ShowString(3,1, "MQTT_ID"); 
	sprintf(command, "AT+MQTTCLIENTID=0,\"%s\"\r\n",clientId);
	// Serial2_SendString(command);             //发送串口2给电脑看指令情况
	ESP8266_Init_Send(command, "OK4", "OR4");

	//发送MQTT连接URL信息
	OLED_ShowString(3,1, "                 ");
	OLED_ShowString(3,1, "MQTT_URL");
	sprintf(command, "AT+MQTTCONN=0,\"%s\",%s,1\r\n",mqttHostUrl,port);
	// Serial2_SendString(command);             //发送串口2给电脑看指令情况
	ESP8266_Init_Send(command, "OK5", "OR5");

	//获取连接的时间服务器
	OLED_ShowString(3,1, "                 ");
	OLED_ShowString(3,1, "TIME_CONN");
	sprintf(command, "AT+CIPSNTPCFG=1,8,\"ntp.aliyun.com\"\r\n");
	// Serial2_SendString(command);             //发送串口2给电脑看指令情况
	ESP8266_Init_Send(command, "OK6", "OR6");

	OLED_ShowString(3,1, "                 ");
	OLED_ShowString(3,1, "TIME_GET");
	Delay_s(1);

	// //获取时间
	// sprintf(command, "AT+CIPSNTPTIME?\r\n");
	// Serial2_SendString(command);             //发送串口2给电脑看指令情况
	// ESP8266_Init_Send(command, "OK7", "OR7");
}


/*************************
 * 函数名称：ESP8266_Time_Get
 * 函数功能：ESP8266的获取网络时间
 * 输入参数：u8 *hour, u8 *minute, u8 *second  时分秒
 * 输出参数：无
 * 调用示例：ESP8266_Time_Get(&hour, &minute, &second);
 **************************/

void ESP8266_Time_Get(u8 *hour, u8 *minute, u8 *second)
{
	//定义一个足够大的字符数组来存储合并后的字符串
	char command[140];
	memset(command, 0, sizeof(command));    //清空command数组
	//获取时间
	sprintf(command, "AT+CIPSNTPTIME?\r\n");
	Serial_SendString(command);             //发送串口1发给ESP8266
	// Serial2_SendString(command);             //发送串口2给电脑看指令情况
	//解析返回的时间
	//定义一个足够大的字符数组来存储接收到的字符串
	char String[170];
	memset(String, 0, sizeof(String));    //清空String数组
	//定义一个计数器
	u8 i = 0;
	while(1)
	{
		//等待串口接收到数据
		while(Serial_GetRxFlag() == 0);
		//将接收到的数据存储到String数组中
		String[i] = Serial_GetRxData();
		//计数器加1
		i++;
		//判断接收到的数据是否为OK
		if(String[i-1] == 'K' && String[i-2] == 'O')
		{
			//数组末端加上换行符
			String[i] = '\r';
			String[i+1] = '\n';
			// Serial2_SendString(String);             //发送串口2给电脑看指令情况

			char *time_start = strstr(String, ":");
			if (time_start == NULL) {
				// 如果没有找到冒号，解析失败，返回
				OLED_ShowString(3,11,"OR7");
				break;
			}
			char *time_second = strstr(time_start+1, ":");
			if (time_second == NULL) {
				// 如果没有找到冒号，解析失败，返回
				OLED_ShowString(3,11,"OR7");
				break;
			}

            // Serial2_SendNumber(*time_second, 2);

			//解析出时间
			char hour_str[2];
			char minute_str[2];
			char second_str[2];
			memset(hour_str, 0, sizeof(hour_str));
			memset(minute_str, 0, sizeof(minute_str));
			memset(second_str, 0, sizeof(second_str));
			strncpy(hour_str, time_second-2, 2);
			strncpy(minute_str, time_second+1, 2);
			strncpy(second_str, time_second+4, 2);

			// Serial2_SendString(hour_str);
			// Serial2_SendString(":");
			// Serial2_SendString(minute_str);
			// Serial2_SendString(":");
			// Serial2_SendString(second_str);

			*hour = atoi(hour_str);
			*minute = atoi(minute_str);
			*second = atoi(second_str);
			
			// Serial2_SendNumber(*hour, 2);
			// Serial2_SendString(":");
			// Serial2_SendNumber(*minute, 2);
			// Serial2_SendString(":");
			// Serial2_SendNumber(*second, 2);
			// Serial2_SendString("\r\n");

			// OLED_ShowNum(4,1,12, 2);
			// OLED_ShowString(4,3,":");
			// OLED_ShowNum(4,4,*minute, 2);
			// OLED_ShowString(4,6,":");
			// OLED_ShowNum(4,7,*second, 2);

			//在OLED上显示OK
			OLED_ShowString(3,11,"OK7");
			//返回1
			// return 1;
			break;
		}
		//判断接收到的数据是否为RO
		else if(String[i-1] == 'R' && String[i-2] == 'O') 
		{	
			//数组末端加上换行符
			String[i] = '\r';
			String[i+1] = '\n';
			// Serial2_SendString(String);             //发送串口2给电脑看指令情况
			//返回0
			// return 0;
			break;
		}
	}

	
	Delay_ms(1000);
}

/*************************
 * 函数名称：Serial_ReceiveString_Flag
 * 函数功能：对串口接收到的数据进行处理
 * 输入参数：无
 * 输出参数：u8  1/0  1表示接收到OK，0表示接收到RO
 * 调用示例：Serial_ReceiveString_Flag();
**************************/
u8 Serial_ReceiveString_Flag(void)
{
	//定义一个足够大的字符数组来存储接收到的字符串
	char String[170];
	memset(String, 0, sizeof(String));    //清空String数组
	//定义一个计数器
	u8 i = 0;
	while(1)
	{
		//等待串口接收到数据
		while(Serial_GetRxFlag() == 0);
		//将接收到的数据存储到String数组中
		String[i] = Serial_GetRxData();
		//计数器加1
		i++;
		//判断接收到的数据是否为OK
		if(String[i-1] == 'K' && String[i-2] == 'O')
		{
			//数组末端加上换行符
			String[i] = '\r';
			String[i+1] = '\n';
			// Serial2_SendString(String);             //发送串口2给电脑看指令情况
			//返回1
			return 1;
		}
		//判断接收到的数据是否为RO
		else if(String[i-1] == 'R' && String[i-2] == 'O') 
		{	
			//数组末端加上换行符
			String[i] = '\r';
			String[i+1] = '\n';
			// Serial2_SendString(String);             //发送串口2给电脑看指令情况
			//返回0
			return 0;
		}
	}
}

/*************************
 * 函数名称：ESP8266_Init_Send
 * 函数功能：初始化ESP8266
 * 输入参数：char *command, char *OK, char *OR  指令，OK，OR
 * 输出参数：无
 * 调用示例：ESP8266_Init_Send(command, "OK1", "OR1");
 **************************/
void ESP8266_Init_Send(char *command, char *OK, char *OR)
{
	//用于判断是否接收到OK的标志位
	u8 OK_OR_flag = 0;               	
	//发送指令
	Serial_SendString(command);
	//检测接收的数据是否为OK,将接收标志位赋值给OK_OR_flag
	OK_OR_flag = Serial_ReceiveString_Flag();  
	//判定接收的最后两个字符是不是OK，如果是
	if (OK_OR_flag == 1)
	{	
		//在OLED上显示OK
		OLED_ShowString(3,11, OK);
	}
	//如果不是，再发送1次指令
	else
	{
		//再发送一次指令
		Delay_ms(1000); 
		Serial_SendString(command);
		//再次检测接收的数据是否为OK,将接收标志位赋值给OK_OR_flag
		OK_OR_flag = Serial_ReceiveString_Flag();
		if (OK_OR_flag == 1)
		{
			//在OLED上显示OK
			OLED_ShowString(3,11, OK);
		}
		else
		{
			//在OLED上显示OR
			OLED_ShowString(3,11, OR);
		}
		
	}
	Delay_ms(1000); 
}


/*************************
 * 函数名称：ESP8266_Send
 * 函数功能：发送数据到云端
 * 输入参数：char *property1, float Data1, char *property2, float Data2, char *property3, u8 Data3, char *property4, u8 Data4, char *property5, u8 Data5, char *property6, u8 Data6, char *property7, u8 Data7, char *property8, u8 Data8
 * 输出参数：无
 * 调用示例：ESP8266_Send("temperature", 25.5, "humidity", 50.5, "light", 1, "hour_set", 12, "minute_set", 30, "second_set", 0, "temp_set", 30, "humi_set", 50);
**************************/

// void ESP8266_Send(char *property1, float Data1, char *property2, float Data2, char *property3, u8 Data3, char *property4, u8 Data4, char *property5, u8 Data5, char *property6, u8 Data6, char *property7, u8 Data7, char *property8, u8 Data8)
// {	
// 	// 定义一个足够大的字符数组来存储合并后的字符串
// 	char command[270];
// 	memset(command, 0, sizeof(command)); 
// 	// 定义消息发送格式
// 	//sprintf(command, "AT+MQTTPUB=0,\"%s\",\"{\\\"params\\\":{\\\"%s\\\":%d.%d}}\",1,0\r\n", post, property, Data_int, Data_decimal);  //第四个参数设置为0表示不要响应
// 	sprintf(command, "AT+MQTTPUB=0,\"%s\",\"{\\\"params\\\":{\\\"%s\\\":%.1f\\, \\\"%s\\\":%.1f\\, \\\"%s\\\":%d\\, \\\"%s\\\":%d\\, \\\"%s\\\":%d\\, \\\"%s\\\":%d\\, \\\"%s\\\":%d\\, \\\"%s\\\":%d\\}}\",0,0\r\n", post, property1, Data1, property2, Data2, property3, Data3, property4, Data4, property5, Data5, property6, Data6, property7, Data7, property8, Data8);  //第四个参数设置为0表示不要响应
// 	// 发送消息
// 	Serial_SendString(command);
// 	Serial2_SendString(command);             //发送串口2给电脑看指令情况

// 	// 在OLED上显示OK_S
// 	// OLED_ShowString(1,8,"OK_S");
// }

void ESP8266_Send(char *property1, float Data1, char *property2, u8 Data2, char *property3, u8 Data3, char *property4, u8 Data4, char *property5, u8 Data5, char *property6, u8 Data6)
{	
	// 定义一个足够大的字符数组来存储合并后的字符串
	char command[260];
	memset(command, 0, sizeof(command)); 
	// 定义消息发送格式
	//sprintf(command, "AT+MQTTPUB=0,\"%s\",\"{\\\"params\\\":{\\\"%s\\\":%d.%d}}\",1,0\r\n", post, property, Data_int, Data_decimal);  //第四个参数设置为0表示不要响应
	sprintf(command, "AT+MQTTPUB=0,\"%s\",\"{\\\"params\\\":{\\\"%s\\\":%.1f\\, \\\"%s\\\":%d\\, \\\"%s\\\":%d\\, \\\"%s\\\":%d\\, \\\"%s\\\":%d\\, \\\"%s\\\":%d\\}}\",0,0\r\n", post, property1, Data1, property2, Data2, property3, Data3, property4, Data4, property5, Data5, property6, Data6);  //第四个参数设置为0表示不要响应
	// 发送消息
	Serial_SendString(command);
	// Serial2_SendString(command);             //发送串口2给电脑看指令情况
	// 释放内存空间
    free(command);

	// 在OLED上显示OK_S
	// OLED_ShowString(1,8,"OK_S");
}

/*************************
 * 函数名称：ESP8266_Received
 * 函数功能：接收云端消息 并解析数据格式
 * 输入参数：char *PRO  属性名
 * 输出参数：无
 * 调用示例：ESP8266_Received("temperature");
 * 数据格式示例: "/sys/k0twlspmAZo/ESP-01S/thing/service/property/set",99,{"method":"thing.service.property.set","id":"159665729","params":{"LEDSwitch":0},"version":"1.0.0"} 解析出LEDSwitch的值
 * **************************/ 
// u8 ESP8266_Received(char *PRO, u8 *value)
// {
// 	//判断串口1中断标志位是否被设置
// 	// if(Serial_GetRxFlag() == 1)
// 	// {
// 		char String[180];
// 		memset(String, 0, sizeof(String));    //清空String数组
// 		Serial_ReceiveString(String);
// 		Serial2_SendString(String);             //发送串口2给电脑看指令情况
// 		//解析出属性名对应的值  例如解析出LEDSwitch的值 {"LEDSwitch":0},
// 		//查找PRO在String中的位置 例如查找字符串 "LEDSwitch" 在String中的位置, 返回指向第一次出现的位置的指针 即字符串第一个字符引号的位置
// 		char *p = strstr(String, PRO);
// 		//如果找到了
// 		if(p != NULL)
// 		{
// 			//获取字符串"LEDSwitch"的长度
// 			u8 len = strlen(PRO);
// 			// Serial2_SendNumber(len, 2);
// 			// Serial2_SendString("\r\n");
// 			//查找第一个}在String中的位置
// 			char *q = strstr(String, "}");
// 			if(q == NULL)
// 			{
// 				return 0;
// 			}
// 			//定义一个足够大的字符数组来存储解析出的数据
// 			char Data[20];
// 			memset(Data, 0, sizeof(Data));                 //清空Data数组 没有这个 第一次会有bug
// 			// Serial2_SendString(Data); 
// 			// Serial2_SendString("\r\n");

// 			//将"LEDSwitch":与"}"的之间的值存储到Data数组中
// 			strncpy(Data, p+len+2, q-p-len-2);  // strncpy(目标数组, 源数组, 复制的长度) 
// 			// strncpy(Data, p+len+2, 1);  // strncpy(目标数组, 源数组, 复制的长度) 
// 			//清空OLED第四行显示
// 			// OLED_ShowString(4,1, "                 ");
// 			// //将Data数组中的数据字符在OLED上显示
// 			// OLED_ShowString(4,1, Data);
// 			// Serial2_SendString(Data); 
// 			// Serial2_SendString("\r\n");

// 			//将Data数组中的数据转化为整数
// 			*value = atoi(Data);  
// 			//清楚所有的字符数组
// 			memset(String, 0, sizeof(String));
// 			memset(Data, 0, sizeof(Data));
// 			return 1;
// 			// memset(Data, 0, sizeof(Data));
// 		}
// 		return 0;		
// 	// }
// }

/********************
 * 函数名称：ESP8266_Received
 * 函数功能：接收云端消息 并解析数据格式
 * 输入参数：u8 *hour, u8 *minute, u8 *second 时分秒 u8 *temp_set, u8 *humi_set 温度湿度阈值
 * 输出参数：无
 * 调用示例：ESP8266_Received(&hour, &minute, &temp_set, &humi_set);
*********************/
u8 ESP8266_Received(u8 *hour_set, u8 *minute_set, u8 *temp_set, u8 *humi_set, u8 *hour_set2, u8 *minute_set2)
// void ESP8266_Received(u8 *hour_set, u8 *minute_set, u8 *second_set)
{

	char String[250];
	memset(String, 0, sizeof(String));    //清空String数组
	Serial_ReceiveString(String);
	// Serial2_SendString(String);             //发送串口2给电脑看指令情况
	//解析出属性名对应的值  例如解析出LEDSwitch的值 {"LEDSwitch":0},
	//查找PRO在String中的位置 例如查找字符串 "LEDSwitch" 在String中的位置, 返回指向第一次出现的位置的指针 即字符串第一个字符引号的位置
	char *params_start = strstr(String, "\"params\":{");

    if (params_start != NULL) {

		char *hour_start = strstr(params_start, "\"hour_set\":");
        if (hour_start != NULL) {
            int hour;
            sscanf(hour_start, "\"hour_set\":%d", &hour);
			*hour_set = hour;
        }

        char *minute_start = strstr(params_start, "\"minute_set\":");
        if (minute_start != NULL) {
            int minute;
            sscanf(minute_start, "\"minute_set\":%d", &minute);
			*minute_set = minute;
        }

		char *temp_set_start = strstr(params_start, "\"temp_set\":");
        if (temp_set_start != NULL) {
            int temp;
            sscanf(temp_set_start, "\"temp_set\":%d", &temp);
			*temp_set = temp;
        }

		char *humi_set_start = strstr(params_start, "\"humi_set\":");
        if (humi_set_start != NULL) {
            int humi;
            sscanf(humi_set_start, "\"humi_set\":%d", &humi);
			*humi_set = humi;
        }

		char *hour_start2 = strstr(params_start, "\"hour_set2\":");
        if (hour_start2 != NULL) {
            int hour2;
            sscanf(hour_start2, "\"hour_set2\":%d", &hour2);
			*hour_set2 = hour2;
        }

        char *minute_start2 = strstr(params_start, "\"minute_set2\":");
        if (minute_start2 != NULL) {
            int minute2;
            sscanf(minute_start2, "\"minute_set2\":%d", &minute2);
			*minute_set2 = minute2;
        }

		return 1;
    }
	return 0;

}
