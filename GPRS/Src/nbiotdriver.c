#include "nbiotdriver.h"
#include "stm32f1xx.h"
#include "usart.h"
#include <string.h>
#include "time.h"
#include "led.h"
#include "stdio.h"
#include "sht20.h"
#include "gps.h"

static uint8_t CurrentRty;//记录当前指令执行次数
static tsTimeType TimeNB;//定义时间状态结构体,用于判断接受指令是否超时
static tsTimeType TimeSend;//定时发送数据
static teATCmdNum ATRecCmdNum;
static teATCmdNum ATCmdNumCurrent;//当前命令
static teATCmdNum ATCmdNumNext;//下一条指令
static teNBTaskStatus NBTaskStatus;
tsMessage Message;
tsMessageBuff GPSMessageBuff;

float heigth;
float temperature;
float humidity;
uint8_t IMEI_Flag = 0;
uint8_t find_string_flag =0;
uint8_t my_sleep = 0; //休眠开关(0关闭)
uint8_t NB_reset_time = 5;                                                 //复位次数


char CSQ_buffer[3] = {1};
char topic_msg[44];
char IMEI_Buff[22];
char NB_SendDataBuff[1024];


tsATCmds ATCmds[] = 
{
	  
		{"ATI\r\n","OK",200,NO_REC,3},																			 //查询模组信息	
		{"AT+CGREG?\r\n","OK",200,NO_REC,10},                                 //查看GPRS注册状态
    {"AT+CGATT?\r\n","OK",200,NO_REC,10},                                 //查看GPRS附着状态
		
    {"AT+MCONFIG=GPRS,,\r\n","OK",200,NO_REC,3},                       		 //设置client ID
		{"AT+MIPSTART=47.105.70.108,1883\r\n","CONNECT",3000,NO_REC,3},      //设置IP端口
    {"AT+MCONNECT=1,500\r\n","CONNACK",500,NO_REC,2},                      //建立MQTT会话

    {"AT+MSUB=/MY_PC,1\r\n","SUBACK",2000,NO_REC,2},                       //订阅主题
    {"AT+MQTTMODE=0\r\n","OK",1000,NO_REC,2},                              //发送消息格式为ASCII
		{"AT+CSQ\r\n","+CSQ:",200,NO_REC,10},                                 //查询信号
    {"AT+MPUB=/My_android,1,0","PUBACK",10000,NO_REC,2},                         //发送消息
    {"AT+MDISCONNECT\r\n","OK",3000,NO_REC,3},                             //关闭MQTT连接

    {"AT+MIPCLOSE\r\n","OK",200,NO_REC,3},                                 //关闭TCP连接
    
		{"AT+RESET\r\n","OK",3000,NO_REC,3},                                   //复位模组
    
};

void ATSend(teATCmdNum ATCmdNum)
{
	char NBSendMsg[500];//发送的字符串消息
	memset(Usart2Type.Usart2RecBuff,0,USART2_REC_SIZE);//清空2接受缓存区
	memset(topic_msg,0,sizeof(topic_msg));
	ATCmds[ATCmdNum].ATStatus = NO_REC;//设置接受状态为未接收
	ATRecCmdNum = ATCmdNum;
	if(ATCmdNum == AT_MPUB)
	{
		RedBuff($GNGGA);
		ParseMessageBuf($GNGGA);
		heigth = atof(Message.Elevation);
		
		Red_Value(HUMIDITY, &humidity);
		Red_Value(TEMP, &temperature);
		memset(NBSendMsg, 0, sizeof(NBSendMsg));
		memset(NB_SendDataBuff, 0, sizeof(NB_SendDataBuff));
		//sprintf(NBSendMsg, "{%f,%f}", lat, lng);
		//sprintf(NBSendMsg,"{%s\\2C%.2f\\2C%.2f\\2C%f\\2C%f\\2C%f}",CSQ_buffer,temperature,humidity,lat,lng,heigth);
		sprintf(NBSendMsg,"{\\22CSQ\\22:%s\\2C\\22temp\\22:%.2f\\2C\\22hum\\22:%.2f\\2C\\22lat\\22:%.6f\\2C\\22lng\\22:%.6f\\2C\\22ele\\22:%.2f}",CSQ_buffer,temperature,humidity,Message.Latitude,Message.Longitude,heigth);
		//printf("##:SEND_buf:%s\r\n",NBSendMsg);
		
		sprintf(NB_SendDataBuff,"%s,%s\r\n",ATCmds[ATCmdNum].ATSendStr,NBSendMsg);
		MyUsartTransmit(NB_SendDataBuff, strlen(NB_SendDataBuff));
		//HAL_UART_Transmit_IT(&huart2, (uint8_t *)NB_SendDataBuff, strlen(NB_SendDataBuff));
		//while(HAL_UART_Transmit_IT(&huart2, (uint8_t *)NB_SendDataBuff, strlen(NB_SendDataBuff)) == HAL_OK);
		printf("##Buff:%s\r\n",NB_SendDataBuff);
		NBTaskStatus = NB_WAIT;
	}else
	{
		HAL_UART_Transmit(&huart2, (uint8_t *)ATCmds[ATCmdNum].ATSendStr, strlen(ATCmds[ATCmdNum].ATSendStr), 0xFF);
		printf("Send:%s\r\n", ATCmds[ATCmdNum].ATSendStr);
	}
	
	SetTime(&TimeNB, ATCmds[ATCmdNum].TimeOut);//每发送一条质指令就打开定时器
}
void ATRec(void)
{
	if(Usart2Type.Usart2RecFlag)
	{
		if(strstr((const char*)Usart2Type.Usart2RecBuff, ATCmds[ATRecCmdNum].ATRecStr) != NULL)//判断接受的字符串是否跟预期接受的相同
		{
			ATCmds[ATRecCmdNum].ATStatus = SUCCESS_REC;
		}
		HAL_UART_Transmit(&huart1,Usart2Type.Usart2RecBuff,Usart2Type.Usart2RecLen,0xFFFF);//串口1打印接受到的数据
		Usart2Type.Usart2RecFlag = 0;//清空接受标志位
		Usart2Type.Usart2RecLen = 0;//清空接受长度
	}
	else if(Usart3Type.Usart3RecFlag)
	{
		//HAL_UART_Transmit(&huart1,Usart3Type.Usart3RecBuff,Usart3Type.Usart3RecLen,0xFFFF);//串口1打印接受到的数据
		Usart3Type.Usart3RecFlag = 0;//清空接受标志位
		Usart3Type.Usart3RecLen = 0;//清空接受长度
		//printf("%s\r\n", "no data receive");
	}
}

void NB_Init(void)
{
		HAL_UART_Transmit(&huart1,(uint8_t *)"NB Init...\r\n",strlen("NB Init...\r\n"),0xFFFF);                 
    NBTaskStatus = NB_SEND;           //进入发送模式
    ATCmdNumCurrent = ATI;              //当前指令
    ATCmdNumNext = (teATCmdNum)(ATCmdNumCurrent+1);        //下一条指令
}

void NB_Task(void)
{
	while(1)
	{
		switch(NBTaskStatus)
		{
			case NB_IDIE://空闲状态
				if(CompareTime(&TimeSend))
				{
					ATCmdNumCurrent = AT_CSQ;
					ATCmdNumNext = AT_IDIE;
					NBTaskStatus = NB_SEND;
					SetTime(&TimeSend, 5000);
					break;
				}
				return;
			case NB_SEND://发送状态
				if(ATCmdNumCurrent != ATCmdNumNext)//如果当前指令不等于下一条指令则认为是第一次执行,否则为重复执行
				{
					CurrentRty = ATCmds[ATCmdNumCurrent].RtyNum;//获取当前指令设定的重发次数
				}
				ATSend(ATCmdNumCurrent); //发送数据
				NBTaskStatus = NB_WAIT;
				return;
			case NB_WAIT://等待数据状态
				ATRec();//接收数据
				DealData();
				return;
			case NB_ACCESS://成功连接服务器
				ATCmdNumCurrent = AT_MPUB;
				ATCmdNumNext = AT_IDIE;
				NBTaskStatus = NB_SEND;
				SetTime(&TimeSend, 5000);
				return;
			default:
				return;
		}
	}
}

void DealData(void)
{
	if(ATCmds[ATCmdNumCurrent].ATStatus == SUCCESS_REC)//成功数据包
	{
		switch (ATCmdNumCurrent)
		{
			case AT_CSQ:
				memset(CSQ_buffer, 0, sizeof(CSQ_buffer));
				find_string_flag = Find_string((char *)Usart2Type.Usart2RecBuff, ": ", ",", CSQ_buffer);
				ATCmdNumCurrent = (teATCmdNum)(ATCmdNumCurrent+1);
				ATCmdNumNext = (teATCmdNum)(ATCmdNumCurrent+1);
				NBTaskStatus = NB_SEND;
				break;
			case AT_MIPSTART:
				ATCmdNumCurrent = AT_MCONNECT;
				ATCmdNumNext = (teATCmdNum)(ATCmdNumCurrent+1);
				NBTaskStatus = NB_SEND;
				break;
			case AT_MCONNECT:
				NBTaskStatus = NB_ACCESS;
				break;
			case AT_MPUB:
				NBTaskStatus = NB_IDIE;
				break;
			default:
				ATCmdNumCurrent = (teATCmdNum)(ATCmdNumCurrent+1);
				ATCmdNumNext = (teATCmdNum)(ATCmdNumCurrent+1);
				NBTaskStatus = NB_SEND;
		}
	}
	else if(CompareTime(&TimeNB))//还没收到预期数据  并且超时
	{
		ATCmds[ATCmdNumCurrent].ATStatus = TIME_OUT;
		if(CurrentRty > 0)
		{
			CurrentRty--;
			ATCmdNumNext = ATCmdNumCurrent;
			NBTaskStatus = NB_SEND;
		}
		else
		{
			ATCmdNumCurrent = AT_RESET;
			NBTaskStatus = NB_SEND;
		}
	}
}
/*
功能:重一串字符串中取出指定字符串
参数:
	pcBuf:一串字符串指针
	left:左边标识指针
	right:右边标识指针
	QpcRes:存储取出的字符串
*/
int Find_string(char *pcBuf,char *left,char *right, char *pcRes)
{
    char *pcBegin = NULL;
    char *pcEnd = NULL;
    pcBegin = strstr(pcBuf, left);
    pcEnd = strstr(pcBegin+strlen(left), right);
    if(pcBegin == NULL || pcEnd == NULL || pcBegin > pcEnd)
    {
        HAL_UART_Transmit(&huart1,(uint8_t *)"string name not found!\r\n",strlen("string name not found!\r\n"),0xFFFF);
				return 0;
    }
    else
    {
        pcBegin += strlen(left);
        memcpy(pcRes, pcBegin, pcEnd-pcBegin);
				HAL_UART_Transmit(&huart1,(uint8_t *)"found string ok\r\n",strlen("string name not found!\r\n"),0xFFFF);
				return 1;
    }
		
}
