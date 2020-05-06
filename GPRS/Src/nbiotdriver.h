#ifndef _NBIOTDRIVER_H
#define _NBIOTDRIVER_H
#include "stm32f1xx.h"
#include <ctype.h>
#include <stdlib.h>
#include "usart.h"


typedef enum
{
	
	ATI = 0,											//查询模组信息
	AT_CGRET,									//查看GPRS注册状态
	AT_CGATT,									//查看GPRS附着状态
	
	AT_MCONFIG,   						//设置client ID   
	AT_MIPSTART, 							//设置IP端口
	AT_MCONNECT,   						//建立MQTT会话
	AT_MSUB,    							//订阅主题
	AT_MQTTMODE,							//发送消息格式为ASCII
	AT_CSQ,   								//查询信号
	AT_MPUB, 	 								//发送消息
	AT_MDISCONNECT,   				//关闭MQTT连接
	AT_MIPCLOSE,   						//关闭TCP连接
	
	AT_RESET, 						//复位模组
	AT_IDIE,
}teATCmdNum;

typedef enum	//AT指令任务状态
{
	NB_IDIE = 0,//空闲
	NB_SEND,//发送
	NB_WAIT,//等待
	NB_ACCESS//入网完成
}teNBTaskStatus;

typedef enum
{
	SUCCESS_REC = 0,
	TIME_OUT,
	NO_REC
}teATStatus;
typedef struct
{
	char *ATSendStr;//AT指令发送的字符串
	char *ATRecStr;//AT指令接受的字符串
	uint16_t TimeOut;//超时
	teATStatus ATStatus;//AT指令状态
	uint8_t RtyNum;//重发次数
}tsATCmds;

void NB_RESET(void);//NB复位
void DealData(void);//数据处理函数
void NB_Init(void);
void NB_Task(void);
int Find_string(char *pcBuf,char *left,char *right, char *pcRes);//在一串字符串中找到指定字符串
#endif

