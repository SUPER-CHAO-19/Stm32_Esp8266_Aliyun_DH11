#ifndef __ESP8266_H
#define __ESP8266_H

/*MQTT 连接参数*/
#define clientId "k0twlspmAZo.ESP-01S|securemode=2\\,signmethod=hmacsha256\\,timestamp=1711532484808|"
#define username "ESP-01S&k0twlspmAZo"
#define passwd "723801c9ffb2823d7ebfcef6dda42970be8081cad25bdeeee35814a619b7aece"
#define mqttHostUrl "iot-06z00bge6iz3an1.mqtt.iothub.aliyuncs.com"
#define port "1883"
#define post "/sys/k0twlspmAZo/ESP-01S/thing/event/property/post"
#define post_name "powerstate"             //还未修改

// #define WIFI_Name "AS"                   
// #define WIFI_Pass "Design@2021"
#define WIFI_Name "Super_Chao IPHONE"                   
#define WIFI_Pass "12345678"
// extern unsigned char xdata Property_Data[];		//数据保存数组



void ESP8266_Init_Send(char *command, char *OK, char *OR);                         //初始化发送函数
void ESP8266_Init(void);                                                           //初始化函数

void ESP8266_Time_Get(u8 *hour, u8 *minute, u8 *second);                           //获取时间函数

u8 Serial_ReceiveString_Flag(void);                                                //串口接收标志位

// void ESP8266_Send(char *property1, float Data1, char *property2, float Data2, char *property3, u8 Data3, char *property4, u8 Data4, char *property5, u8 Data5, char *property6, u8 Data6, char *property7, u8 Data7, char *property8, u8 Data8);     //发送函数

void ESP8266_Send(char *property1, float Data1, char *property2, u8 Data2, char *property3, u8 Data3, char *property4, u8 Data4, char *property5, u8 Data5, char *property6, u8 Data6);

u8 ESP8266_Received(u8 *hour_set, u8 *minute_set, u8 *temp_set, u8 *humi_set, u8 *hour_set2, u8 *minute_set2);                        //接收函数

// void ESP8266_Received(u8 *hour_set, u8 *minute_set, u8 *second_set);  
#endif
